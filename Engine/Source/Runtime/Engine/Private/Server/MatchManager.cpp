#include "Server/MatchManager.h"
#include <cstring>
#include <chrono>
#include "Network/NetBitWriter.h"

namespace tomato
{
	void MatchManager::Update(float dt)
	{
		ProcessMatchRequest();

		// Create a match when people get together
		if (CheckPopulation())
		{
			MatchContext ctx;
			CreateMatchContext(ctx);
			matches.try_emplace(ctx.matchId, ctx);

			for (const auto& c : ctx.players)
				std::cout << c.sessionId << " ";
		}

		ProcessMatchResult(dt);
	}

	void MatchManager::ProcessMatchRequest()
	{
		while (!MatchRequestQueue.Empty())
		{
			MatchRequestCommand reqCommand;
			MatchRequestQueue.Dequeue(reqCommand);

			if (reqCommand.action == MatchRequestAction::Enqueue)
				HandleEnqueue(reqCommand.sessionId, reqCommand.address, reqCommand.name);

			if (reqCommand.action == MatchRequestAction::Cancel)
				HandleCancel(reqCommand.sessionId);

			if (reqCommand.action == MatchRequestAction::Success)
				HandleIntroResult(reqCommand.sessionId, reqCommand.matchId, 1);

			if (reqCommand.action == MatchRequestAction::Failed)
				HandleIntroResult(reqCommand.sessionId, reqCommand.matchId, 0);
		}
	}

	void MatchManager::HandleEnqueue(const SessionId& client, const SocketAddress& inAddress, const std::string& name)
	{
		MatchRequest mRequest{
			.sessionId = client,
			.address = inAddress,
			.name = name,
			.requestId = 0,
			.enqueueTime = 0,
			//duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
			.retryCount = 0,
		};

		requests.try_emplace(client, mRequest);
		pq.emplace(mRequest);
	}

	void MatchManager::HandleCancel(const SessionId& client)
	{
		/* @TODO:
		* Match cancellation during active matching is not fully handled.
		* For now, we only remove the request from the map.
		* Proper cancellation logic is needed when a match is already being created.
		*/

		auto it = requests.find(client);
		if (it != requests.end())
			requests.erase(it);
	}

	void MatchManager::HandleIntroResult(const SessionId& client, MatchId& matchId, const int& set)
	{

		std::cout << __FUNCTION__ << '\n';
		auto it = matches.find(matchId);
		if (it != matches.end())
		{
			int idx = it->second.GetPlayerId(client);
			if (idx < 0) return;
			std::cout << __FUNCTION__ << '\n';
			it->second.SetPeerAck(idx, set);
		}
	}

	bool MatchManager::CheckPopulation()
	{
		if (requests.size() >= MatchConstants::MAX_MATCH_PLAYER)
			return true;
		return false;
	}

	bool MatchManager::GetMatchRequestFromPQ(MatchRequest& req)
	{
		while (!pq.empty())
		{
			MatchRequest pReq = pq.top();
			pq.pop();

			if (!requests.contains(pReq.sessionId))
				continue; // lazy deletion

			else
			{
				req = pReq;
				return true;
			}
		}
		return false;
	}

	bool MatchManager::CreateMatchContext(MatchContext& ctx)
	{
		ctx.matchId = nextMatchID_++;
		ctx.state = MatchState::Init;

		MatchRequest out;
		for (int i = 0; i < MatchConstants::MAX_MATCH_PLAYER; i++)
		{
			if (GetMatchRequestFromPQ(out))
			{
				ctx.players[i] = out;
				requests.erase(out.sessionId);
			}
		}

		return true;
	}

	void MatchManager::ProcessMatchResult(float dt)
	{
		// Update the state of all matches (Separating as a function)
		for (auto it = matches.begin(); it != matches.end();)
		{
			MatchUpdateResult res = it->second.Update(dt, NetSendRequestQueue);

			switch (res)
			{
			case MatchUpdateResult::None:
			{
				++it;
				break;
			}

			case MatchUpdateResult::ReadyToStart:
			{
				//SendPacket for game start
				//then, Remove from matches
				std::cout << "[MatchState::ALLREADY] Match ReadyToStart\n";
				ServerTimeMs serverSteadyTime = static_cast<ServerTimeMs>(
					duration_cast<std::chrono::milliseconds>(
						std::chrono::steady_clock::now().time_since_epoch()
					).count());
				ServerTimeMs startServerTime = serverSteadyTime + MatchConstants::START_SERVER_TIME;

				const MatchRequest* req = it->second.GetMatchRequest();

				RawBuffer buf{};
				tomato::NetBitWriter writer{ &buf };
				writer.WriteInt(static_cast<uint16_t>(0), std::numeric_limits<uint16_t>::max());
				writer.WriteInt(static_cast<uint16_t>(TCPPacketType::MATCH_START), static_cast<uint16_t>(TCPPacketType::COUNT));
				writer.WriteInt(startServerTime, std::numeric_limits<ServerTimeMs>::max());
				uint16_t size = writer.GetByteSize();

				memcpy(buf.data(), &size, sizeof(uint16_t));

				for (int i = 0; i < MatchConstants::MAX_MATCH_PLAYER; i++)
				{
					auto sendCmd = std::make_unique<SendRequestCommand>((req + i)->sessionId, size);
					std::memcpy(sendCmd.get()->data.data(), buf.data(), size);

					NetSendRequestQueue.Emplace(std::move(sendCmd));
				}

				it = matches.erase(it);
				break;
			}

			// When peers can't connect or time out
			case MatchUpdateResult::Failed:
			{
				// Remove from matches after ReQueing
				ReQueing(it->first);
				it = matches.erase(it);

				std::cout << "ReQueing\n";
				break;
			}
			}
		}
	}

	void MatchManager::ReQueing(const MatchId& matchId)
	{
		auto it = matches.find(matchId);
		if (it != matches.end())
		{
			const MatchRequest* request = it->second.GetMatchRequest();

			for (int i = 0; i < MatchConstants::MAX_MATCH_PLAYER; i++)
			{
				MatchRequest reRequest
				{
					.sessionId = (request + i)->sessionId,
					.name = (request + i)->name,
					.requestId = (request + i)->requestId,
					.enqueueTime = static_cast<ServerTimeMs>(
						duration_cast<std::chrono::milliseconds>(
							std::chrono::steady_clock::now().time_since_epoch()
						).count()
					),
					.retryCount = (request + i)->retryCount + 1,
				};

				if (reRequest.retryCount > 3)
					continue;

				requests.try_emplace(reRequest.sessionId, reRequest);
				pq.emplace(reRequest);
			}
		}
	}
}
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
		if (CheckPopulation() || CheckMatchTime(dt))
		{
			MatchContext ctx;
			CreateMatchContext(ctx);
			matches.try_emplace(ctx.matchId, ctx);
			matchPlayerNum = 0;

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

			if (reqCommand.action == MatchRequestAction::GameEnd)
				HandleGameEnd(reqCommand.sessionId, reqCommand.matchId);

			if (reqCommand.action == MatchRequestAction::Leave)
				HandleLeave(reqCommand.sessionId, reqCommand.matchId);

			if (reqCommand.action == MatchRequestAction::Disconnect)
				HandleDisconnect(reqCommand.sessionId);
		}
	}

	void MatchManager::HandleEnqueue(SessionId client, const SocketAddress& inAddress, const std::string& name)
	{
		if (clientInfo_.contains(client))
			return;

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
		clientInfo_[client] = { ClientMatchState::Queued, 0 };
		pq.emplace(mRequest);

		waitTime = 0.f;
	}

	void MatchManager::HandleCancel(SessionId client)
	{
		/* @TODO:
		* Match cancellation during active matching is not fully handled.
		* For now, we only remove the request from the map.
		* Proper cancellation logic is needed when a match is already being created.
		*/

		/*auto it = requests.find(client);
		if (it != requests.end())
		{
			requests.erase(it);
			clientInfo_.erase(client);
		}*/

		auto it = clientInfo_.find(client);
		if (it == clientInfo_.end())
		{
			SendCancelResult(client, false); // Idle
			return;
		}

		ClientMatchInfo clientInfo = it->second;
		switch (clientInfo.state)
		{
		case ClientMatchState::Idle: // 사실상 도달하지 않음 (Idle은 맵에 없음)
			break;

		case ClientMatchState::Queued:
			requests.erase(client);
			clientInfo_.erase(client); // 취소한 사람은 Idle로 변경 (재큐잉 대상에서 제외)
			SendCancelResult(client, true);
			break;

		case ClientMatchState::Matching:
			HandleMatchingCancel(client, clientInfo.matchId); // client failed 처리
			clientInfo_.erase(client); 
			SendCancelResult(client, true);
			break;

		case ClientMatchState::InGame: // 게임 중 취소 불가
			SendCancelResult(client, false); // 실패 응답 전송
			break;

		default:
			break;
		}
	}

	void MatchManager::HandleIntroResult(SessionId client, MatchId matchId, int set)
	{
		std::cout << __FUNCTION__ << '\n';
		auto it = matches.find(matchId);
		if (it != matches.end())
		{
			int idx = it->second.GetPlayerId(client);
			if (idx < 0)
				return;

			std::cout << __FUNCTION__ << '\n';
			it->second.SetPeerAck(idx, set);
		}
	}

	void MatchManager::HandleMatchingCancel(SessionId client, MatchId matchId)
	{
		HandleIntroResult(client, matchId, 0);
	}

	void MatchManager::HandleGameEnd(SessionId client, MatchId matchId)
	{
		auto ci = clientInfo_.find(client);
		if (ci == clientInfo_.end() ||
			ci->second.state != ClientMatchState::InGame ||
			ci->second.matchId != matchId)
			return; //오래된 패킷 무시

		MarkPlayer(client, matchId, MatchPlayerState::Finished);
		clientInfo_.erase(client);

		auto it = activeMatches_.find(matchId);
		if (it == activeMatches_.end())
			return;

		FinishIfDone(matchId);
	}

	void MatchManager::HandleLeave(SessionId client, MatchId matchId)
	{
		auto ci = clientInfo_.find(client);
		if (ci == clientInfo_.end() ||
			ci->second.state != ClientMatchState::InGame ||
			ci->second.matchId != matchId)
			return; //오래된 패킷 무시

		// matchId에 참여한 client의 state를 갱신
		MarkPlayer(client, matchId, MatchPlayerState::Left);
		clientInfo_.erase(client); // 게임 종료된 client의 상태를 Idle로 갱신

		auto it = activeMatches_.find(matchId);
		if (it == activeMatches_.end())
			return;

		auto& players = it->second.players;
		auto& states = it->second.playerStates;

		// 끊긴 플레이어 index
		size_t leftIdx = players.size();
		for (size_t i = 0; i < players.size(); ++i)
		{
			if (players[i] == client)
			{
				leftIdx = i;
				break;
			}
		}
		if (leftIdx == players.size())
			return;

		states[leftIdx] = MatchPlayerState::Left;

		for (size_t i = 0; i < players.size(); ++i)
		{
			if (states[i] == MatchPlayerState::InGame)
				SendPeerStatus(players[i], matchId, static_cast<uint8_t>(leftIdx), PeerState::Left);
		}

		FinishIfDone(matchId);
		clientInfo_.erase(client);
	}

	void MatchManager::HandleDisconnect(SessionId client)
	{
		auto it = clientInfo_.find(client);
		if (it == clientInfo_.end())
			return;

		const ClientMatchState state = it->second.state;
		MatchId localMatchId = it->second.matchId;

		switch (state)
		{
		case ClientMatchState::Queued:
			requests.erase(client);
			break;

		case ClientMatchState::Matching:
			HandleMatchingCancel(client, it->second.matchId); // client failed 처리
			break;

		case ClientMatchState::InGame:
		{
			auto mit = activeMatches_.find(localMatchId);
			if (mit == activeMatches_.end())
				break;

			auto& players = mit->second.players;
			auto& states = mit->second.playerStates;

			// 끊긴 플레이어 index
			size_t leftIdx = players.size();
			for (size_t i = 0; i < players.size(); ++i)
			{
				if (players[i] == client)
				{
					leftIdx = i;
					break;
				}
			}
			if (leftIdx == players.size())
				break;

			// 게임 중 연결이 끊긴 플레이어의 state 갱신
			states[leftIdx] = MatchPlayerState::Disconnected;

			// 남은 InGame 플레이어에게 알림
			for (size_t i = 0; i < players.size(); ++i)
			{
				if (states[i] == MatchPlayerState::InGame)
					SendPeerStatus(players[i], localMatchId, static_cast<uint8_t>(leftIdx), PeerState::Disconnected);
			}

			FinishIfDone(localMatchId);
			break;
		}

		default:
			break;
		}
		
		clientInfo_.erase(client);
	}

	bool MatchManager::MarkPlayer(SessionId client, MatchId matchId, MatchPlayerState state)
	{
		auto it = activeMatches_.find(matchId);
		if (it == activeMatches_.end())
			return false;

		auto& players = it->second.players;
		for (size_t i = 0; i < players.size(); ++i)
		{
			if (players[i] == client)
			{
				it->second.playerStates[i] = state;
				return true;
			}
		}
		return false;
	}

	void MatchManager::SendCancelResult(SessionId to, bool result)
	{
		TCPPacketType resultType = result == true ? TCPPacketType::MATCH_CANCEL_SUCCESS : TCPPacketType::MATCH_CANCEL_FAILED;

		RawBuffer buf{};
		tomato::NetBitWriter writer{ &buf };
		writer.WriteInt(static_cast<uint16_t>(0), std::numeric_limits<uint16_t>::max()); // packet size 자리 확보
		writer.WriteInt(static_cast<uint16_t>(resultType), static_cast<uint16_t>(TCPPacketType::COUNT)); // packet header (type)

		// write packet size
		uint16_t size = writer.GetByteSize();
		memcpy(buf.data(), &size, sizeof(uint16_t));

		auto sendCmd = std::make_unique<SendRequestCommand>(to, size);
		std::memcpy(sendCmd.get()->data.data(), buf.data(), size);

		NetSendRequestQueue.Emplace(std::move(sendCmd));
	}

	void MatchManager::SendPeerStatus(SessionId to, MatchId matchId, uint8_t targetPlayerId, PeerState peerState)
	{
		RawBuffer buf{};
		tomato::NetBitWriter writer{ &buf };
		writer.WriteInt(static_cast<uint16_t>(0), std::numeric_limits<uint16_t>::max()); // packet size 자리 확보
		writer.WriteInt(static_cast<uint16_t>(TCPPacketType::MATCH_PEER_STATUS), static_cast<uint16_t>(TCPPacketType::COUNT)); // packet header (type)
		writer.WriteInt(static_cast<uint16_t>(matchId), std::numeric_limits<uint16_t>::max()); // Match Id
		writer.WriteInt(static_cast<uint8_t>(targetPlayerId), std::numeric_limits<uint8_t>::max());
		writer.WriteInt(static_cast<uint8_t>(peerState), static_cast<uint8_t>(PeerState::COUNT));

		// write packet size
		uint16_t size = writer.GetByteSize();
		memcpy(buf.data(), &size, sizeof(uint16_t));

		auto sendCmd = std::make_unique<SendRequestCommand>(to, size);
		std::memcpy(sendCmd.get()->data.data(), buf.data(), size);

		NetSendRequestQueue.Emplace(std::move(sendCmd));
	}

	void MatchManager::FinishIfDone(MatchId matchId)
	{
		auto it = activeMatches_.find(matchId);
		if (it == activeMatches_.end())
			return;

		for (auto s : it->second.playerStates)
		{
			if (s == MatchPlayerState::InGame)
				return; // 아직 게임 중인 사람이 있음

		}
		activeMatches_.erase(it);
	}

	bool MatchManager::CheckPopulation()
	{
		if (requests.size() >= MatchConstants::MAX_MATCH_PLAYER)
		{
			waitTime = 0.f;
			matchPlayerNum = MatchConstants::MAX_MATCH_PLAYER;

			return true;
		}
		return false;
	}

	bool MatchManager::CheckMatchTime(float dt)
	{
		waitTime += dt;
		if (requests.size() > 0 && waitTime >= MatchConstants::MATCH_WAIT_TIME)
		{
			waitTime = 0.f;
			matchPlayerNum = requests.size();

			return true;
		}

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
		ctx.playerNum = matchPlayerNum;

		/* 매치 대기열에서 우선 순위가 높은 플레이어부터 새 매치 넣기 */
		MatchRequest out;
		for (int i = 0; i < matchPlayerNum; i++)
		{
			if (GetMatchRequestFromPQ(out))
			{
				ctx.players.push_back(out);
				clientInfo_[out.sessionId] = { ClientMatchState::Matching, ctx.matchId };

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

				const std::vector<MatchRequest>& req = it->second.GetMatchRequest();

				RawBuffer buf{};
				tomato::NetBitWriter writer{ &buf };
				writer.WriteInt(static_cast<uint16_t>(0), std::numeric_limits<uint16_t>::max());
				writer.WriteInt(static_cast<uint16_t>(TCPPacketType::MATCH_START), static_cast<uint16_t>(TCPPacketType::COUNT));
				writer.WriteInt(startServerTime, std::numeric_limits<ServerTimeMs>::max());
				uint16_t size = writer.GetByteSize();

				memcpy(buf.data(), &size, sizeof(uint16_t));

				for (int i = 0; i < it->second.GetMatchPlayerNum(); i++)
				{
					auto sendCmd = std::make_unique<SendRequestCommand>(req[i].sessionId, size);
					std::memcpy(sendCmd.get()->data.data(), buf.data(), size);

					NetSendRequestQueue.Emplace(std::move(sendCmd));
				}

				// 활성 게임 관리를 위해 activeMatches_ 매치 데이터를 보관하고,
				// Match와 Client 상태를 InGame으로 갱신
				ActiveMatch am;
				am.matchId = it->first;
				for (int i = 0; i < it->second.GetMatchPlayerNum(); i++)
				{
					am.players.push_back(req[i].sessionId);
					am.playerStates.push_back(MatchPlayerState::InGame);
					clientInfo_[req[i].sessionId].state = ClientMatchState::InGame;
				}
				activeMatches_.emplace(am.matchId, std::move(am));

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
			const std::vector<MatchRequest>& request = it->second.GetMatchRequest();
			uint8_t playerNum = it->second.GetMatchPlayerNum();
			for (int i = 0; i < playerNum; i++)
			{
				/* 연결이 끊긴 세션은 큐에 다시 넣지 않음 */
				if (!clientInfo_.contains(request[i].sessionId))
					continue;

				MatchRequest reRequest
				{
					.sessionId		= request[i].sessionId,
					.address		= request[i].address,
					.name			= request[i].name,
					.requestId		= request[i].requestId,
					.enqueueTime	= static_cast<ServerTimeMs>(
									duration_cast<std::chrono::milliseconds>(
										std::chrono::steady_clock::now().time_since_epoch()
									).count()),
					.retryCount		= request[i].retryCount + 1,
				};

				if (reRequest.retryCount > 3)
				{
					clientInfo_.erase(reRequest.sessionId);
					continue;
				}

				requests.try_emplace(reRequest.sessionId, reRequest);
				clientInfo_[reRequest.sessionId].state = ClientMatchState::Queued;

				pq.emplace(reRequest);
			}
		}
	}

	void MatchManager::RemoveRequest(const SessionId& client)
	{
		for (auto it = requests.begin(); it != requests.end();)
		{
			if (it->first == client)
				it = requests.erase(it);
			else
				++it;
		}
	}
	void MatchManager::PrintActiveMatches()
	{
		std::cout << "\n========== Matches ==========\n";

		for (const auto& [matchId, match] : matches)
		{
			std::cout << "Match ID: " << matchId << '\n';
		}

		std::cout << "=============================\n";

		std::cout << "\n========== Active Matches ==========\n";

		for (const auto& [matchId, match] : activeMatches_)
		{
			std::cout << "Match ID: " << matchId << '\n';
		}

		std::cout << "====================================\n";
	}
}
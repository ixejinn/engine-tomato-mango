#include "Server/MatchServerNetwork.h"

#include <iostream>
#include <vector>
#include <limits>
#include <thread>

#include "Network/NetBitReader.h"
#include "Network/NetBitWriter.h"
#include "Server/SessionManager.h"
#include "Server/MatchManager.h"

namespace tomato
{
	MatchServerNetwork::MatchServerNetwork(SessionManager& sessionMgr, MatchManager& matchMgr, SPSCQueue<MatchRequestCommand, 128>& requestQ, SPSCQueue<SendCommandPtr, 256>& netSendRequestQ)
		:
		sessionMgr_(sessionMgr),
		matchMgr_(matchMgr),
		MatchRequestQueue(requestQ),
		NetSendRequestQueue(netSendRequestQ)
	{
		tcpDriver_.InitDedicatedServerMode();
	};

	void MatchServerNetwork::Update()
	{
		ProcessQueuedPackets();
		ProcessNetSendRequest();
		ProcessSendPacket();
	}

	void MatchServerNetwork::ProcessQueuedPackets()
	{
		//패킷 헤더 해석 및 처리 / 패킷 헤더에 따라 패킷 처리 후 pop
		while (!TCPRecvQueue.Empty())
		{
			TCPPacketPtr nextPacket;
			TCPRecvQueue.Dequeue(nextPacket);

			uint16_t type;
			NetBitReader reader(nextPacket->buffer.data(), nextPacket->size());
			reader.ReadInt(type, static_cast<uint16_t>(TCPPacketType::COUNT));

			if (type == static_cast<uint16_t>(TCPPacketType::SESSION_DISCONNECTED))
				HandlePacketDisconnect(nextPacket->sessionId);
			else
				ProcessPacket(static_cast<TCPPacketType>(type), reader, nextPacket->sessionId);
		}
	}

	void MatchServerNetwork::ProcessPacket(TCPPacketType header, NetBitReader& reader, SessionId client)
	{
		std::cout << __FUNCTION__;
		switch (header)
		{
		case TCPPacketType::GAME_END:
			break;

		case TCPPacketType::MATCH_LEAVE:
			break;

		case TCPPacketType::MATCH_REQUEST:
		case TCPPacketType::MATCH_CANCEL:
		case TCPPacketType::MATCH_INTRO_SUCCESS:
		case TCPPacketType::MATCH_INTRO_FAILED:
			std::cout << "::MATCH_PACKET\n";
			HandlePacketRequest(header, reader, client);
			break;

		case TCPPacketType::TIME_SYNC_REQ:
			std::cout << "::TIME_SYNC_REQ\n";
			HandlePacketTimeSync(header, reader, client);
			break;
		}
	}

	void MatchServerNetwork::HandlePacketDisconnect(SessionId client)
	{
		// 매칭/게임에 포함된 client 처리를 위해 emplace
		MatchRequestQueue.Emplace(client, SocketAddress{}, "", 0, MatchRequestAction::Disconnect);

		sessionMgr_.RemoveSession(client);
	}
	
	void MatchServerNetwork::HandlePacketRequest(TCPPacketType header, NetBitReader& reader, SessionId client)
	{
		uint16_t nameSize{}, port{};
		std::string name{};
		uint8_t tmp;
		MatchId matchId{ 0 };

		SocketAddress addr = sessionMgr_.GetAddress(client);

		switch (header)
		{
		case TCPPacketType::GAME_END:
		{
			// match Id
			reader.ReadInt(matchId, std::numeric_limits<uint16_t>::max());
			MatchRequestQueue.Emplace(client, sessionMgr_.GetAddress(client), name, matchId, MatchRequestAction::GameEnd);
			break;
		}

		case TCPPacketType::MATCH_LEAVE:
		{
			reader.ReadInt(matchId, std::numeric_limits<uint16_t>::max());
			MatchRequestQueue.Emplace(client, sessionMgr_.GetAddress(client), name, matchId, MatchRequestAction::Leave);
			break;
		}

		case TCPPacketType::MATCH_REQUEST:
		{
			// name size + name + udp port number
			reader.ReadInt(nameSize, std::numeric_limits<uint16_t>::max());
			for (int i = 0; i < nameSize; i++)
			{
				reader.ReadInt(tmp, std::numeric_limits<uint8_t>::max());
				name += tmp;
			}
			reader.ReadInt(port, std::numeric_limits<uint16_t>::max());

			sessionMgr_.SetSessionPort(client, port);

			MatchRequestQueue.Emplace(client, sessionMgr_.GetAddress(client), name, matchId, MatchRequestAction::Enqueue);
			break;
		}

		case TCPPacketType::MATCH_INTRO_SUCCESS:
		{
			reader.ReadInt(matchId, std::numeric_limits<MatchId>::max());
			MatchRequestQueue.Emplace(client, addr, name, matchId, MatchRequestAction::Success);
			break;
		}

		case TCPPacketType::MATCH_CANCEL:
		{
			MatchRequestQueue.Emplace(client, addr, name, matchId, MatchRequestAction::Cancel);
			break;
		}

		case TCPPacketType::MATCH_INTRO_FAILED:
		{
			reader.ReadInt(matchId, std::numeric_limits<MatchId>::max());
			MatchRequestQueue.Emplace(client, addr, name, matchId, MatchRequestAction::Failed);
			break;
		}
		}
	}

	void MatchServerNetwork::HandlePacketTimeSync(TCPPacketType header, NetBitReader& reader, SessionId client)
	{
		switch (header)
		{
		case TCPPacketType::TIME_SYNC_REQ:
		{
			ServerTimeMs serverSteadyNow = static_cast<ServerTimeMs>(
				duration_cast<std::chrono::milliseconds>(
					std::chrono::steady_clock::now().time_since_epoch()).count());

			RawBuffer buf{};
			tomato::NetBitWriter writer{ &buf };
			writer.WriteInt(static_cast<uint16_t>(0), std::numeric_limits<uint16_t>::max());
			writer.WriteInt(static_cast<uint16_t>(TCPPacketType::TIME_SYNC_RES), static_cast<uint16_t>(TCPPacketType::COUNT));
			writer.WriteInt(static_cast<uint32_t>(serverSteadyNow), std::numeric_limits<ServerTimeMs>::max());

			uint16_t size = writer.GetByteSize();
			// WARNING:
			// Packet size must be the first field in the buffer.
			// Overwriting this value may behave differently depending on platform/endianness.
			memcpy(buf.data(), &size, sizeof(uint16_t));

			sessionMgr_.AppendSendBuffer(client, buf.data(), size);

			std::cout << "[send] : " << client << " " << int(TCPPacketType::TIME_SYNC_RES) << " " << serverSteadyNow << '\n';
			break;
		}
		}
	}

	void MatchServerNetwork::ProcessNetSendRequest()
	{
		while (!NetSendRequestQueue.Empty())
		{
			SendCommandPtr sendCommand;
			NetSendRequestQueue.Dequeue(sendCommand);

			sessionMgr_.AppendSendBuffer(sendCommand->sessionId, sendCommand->data.data(), sendCommand->size);

			//tomato::NetBitReader rd{ sendCommand->data.data(), static_cast<int16_t>(sendCommand->size) };
			//uint16_t readSize{ 0 }, readType{ 0 };
			//rd.ReadInt(readSize, std::numeric_limits<uint16_t>::max());
			//rd.ReadInt(readType, static_cast<uint16_t>(TCPPacketType::COUNT));
			std::cout << __FUNCTION__ << '\n';
			//std::cout << sendCommand->sessionId << " " << int(readSize) << " " << int(readType) << '\n';
		}
	}

	void MatchServerNetwork::ProcessSendPacket()
	{
		std::vector<TCPSocketPtr> writableSockets;
		sessionMgr_.GetWritableSockets(writableSockets);

		if (writableSockets.empty())
			return;

		for (const TCPSocketPtr& socket : writableSockets)
		{
			if (!sessionMgr_.ValidateSession(socket))
				continue;

			Session* session = sessionMgr_.GetSession(socket);
			if (!session) continue;
			if (session->sendBuffer.empty()) continue;

			int sent = socket->Send(session->sendBuffer.data(), session->sendBuffer.size());
			if (sent > 0)
			{
				std::cout << __FUNCTION__ << '\n';
				//
				//tomato::NetBitReader rd{ session->sendBuffer.data(), static_cast<int16_t>(session->sendBuffer.size()) };
				//uint16_t readSize{ 0 }, readType{ 0 };

				//rd.ReadInt(readSize, std::numeric_limits<uint16_t>::max());
				//rd.ReadInt(readType, static_cast<uint16_t>(TCPPacketType::COUNT));

				//std::cout << int(readSize) << "type : " << int(readType) << '\n';

				session->ConsumeSendBuffer(sent);
			}
		}
	}

	void MatchServerNetwork::ProcessDataFromClient(SessionId sessionId, const uint8_t* data, const int len)
	{
		//socket에 따라 세션에 지정된 vector에 데이터스트림 추가
		if (!sessionMgr_.ValidateSession(sessionId))
			return;

		//client buffer에 데이터스트림 누적 후, 추출할 사이즈만큼 쌓이면 추출 후 큐에 emplace
		auto packet = sessionMgr_.AppendRecvBuffer(sessionId, data, len);
		if (packet == nullptr)
			return;

		TCPRecvQueue.Emplace(std::move(packet));
	}

	void MatchServerNetwork::TCPRecvThreadLoop()
	{
		TCPSocketPtr listenSocket = tcpDriver_.GetSocket();
		std::vector<TCPSocketPtr> readBlockSockets;
		readBlockSockets.push_back(listenSocket);
		std::vector<TCPSocketPtr> readableSockets;

		uint8_t segment[MAX_PACKET_SIZE]{};
		while (isThreadRunning)
		{
			if (!SocketUtil::Select(&readBlockSockets, &readableSockets,
				nullptr, nullptr, nullptr, nullptr))
				continue;

			for (const TCPSocketPtr& socket : readableSockets)
			{
				if (socket == listenSocket) // 새로운 연결 처리
				{
					SocketAddress newClientAddress;
					auto newSocket = listenSocket->Accept(newClientAddress);
					if (newSocket)
					{
						readBlockSockets.push_back(newSocket);
						sessionMgr_.GenerateSession(newSocket, newClientAddress);

						std::cout << "[New Connect] " << newClientAddress.ToString() <<
							" | session num." << sessionMgr_.GetSessionId(newSocket) << '\n';
					}
				}
				else // 기존 연결에서 수신한 데이터 처리
				{
					SessionId id = sessionMgr_.GetSessionId(socket);
					int dataReceived = socket->Receive(segment, MAX_PACKET_SIZE);

					if (dataReceived > 0)
					{
						std::cout << "GetMessage from " << id << '\n';
						ProcessDataFromClient(id, segment, dataReceived);
					}
					else // 연결 끊김 처리
					{
						std::cout << "[Disconnected] " << sessionMgr_.GetAddress(id).ToString() << " | session num." << id << '\n';

						readBlockSockets.erase(
							std::remove(readBlockSockets.begin(), readBlockSockets.end(), socket),
							readBlockSockets.end());

						// 메인 스레드로 세션 끊김 이벤트 전달: 세션 삭제 요청
						RawBuffer rawBuffer{};
						NetBitWriter writer{ &rawBuffer };
						writer.WriteInt(static_cast<uint16_t>(TCPPacketType::SESSION_DISCONNECTED), static_cast<uint16_t>(TCPPacketType::COUNT));

						const uint16_t byteSize = writer.GetByteSize();
						TCPPacketPtr packet = std::make_unique<TCPPacket>(byteSize, id);
						std::memcpy(packet->buffer.data(), rawBuffer.data(), byteSize);
						
						TCPRecvQueue.Emplace(std::move(packet));
					}
				}
				//readableSockets = readBlockSockets;
			}
		}
	}

	void MatchServerNetwork::ThreadStart()
	{
		isThreadRunning = true;
		recvThread = std::thread(&MatchServerNetwork::TCPRecvThreadLoop, this);
	}

	void MatchServerNetwork::ThreadStop()
	{
		isThreadRunning = false;
		recvThread.join();
	}
}
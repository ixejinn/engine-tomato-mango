#include "Network/ClientNetwork.h"
#include "Network/NetBitReader.h"
#include "Network/NetBitWriter.h"
#include "GameNetwork/Message/InputMessage.h"
#include "Utils/Logger.h"

namespace tomato
{
    ClientNetwork::ClientNetwork()
        : playerID_(0), netState_(ClientNetworkState::NSS_Uninitialized) {}

    ClientNetwork::~ClientNetwork()
    {
        // @TODO: 게임 종료 시 GAME_END, 의도적 나가기 시 MATCH_LEAVE 서버로 보내는 동작 수정하기
        if (netState_ == ClientNetworkState::NSS_Playing && connectedToServer)
            LeaveMatch();
            //SendTCPPacket(TCPPacketType::MATCH_LEAVE);
    }

    bool ClientNetwork::Update()
    {
        ProcessQueuedTCPPacket();
        ProcessQueuedUDPPacket();

        if (serverLost_.exchange(false)) // 옛 값이 true였으면 진입, 동시에 값은 false가 됨, 스레드 끼어들기 방지
            pendingReset_ = true;

        if (!pendingReset_)
            return false;

        const bool hadMatch = (matchID_ != 0);
        if (gameNetSystem_)
            gameNetSystem_->OnMatchEnded();
        ResetMatchState();
        pendingReset_ = false;

        return hadMatch;
    }

    bool ClientNetwork::ConnectToServer()
    {
        if (connectedToServer)
            return true;

        if (TCPRecvThread.joinable())
        {
            TCPRecvThread.join();
            recvBuffer.clear();
        }

        if (!server_.InitClientMode())
            return false;

        connectedToServer = true;
        netState_ = ClientNetworkState::NSS_Idle;

        TCPRecvThreadRunning_ = true;
        TCPRecvThread = std::thread(&ClientNetwork::TCPNetRecvThreadLoop, this);

        return true;
    }

    void ClientNetwork::RequestMatch()
    {
        if (!connectedToServer)
        {
            TMT_INFO << "NOT CONNECT TO SERVER";
            return;
        }
        
        if (netState_ != ClientNetworkState::NSS_Idle)
            return;

        SendTCPPacket(TCPPacketType::MATCH_REQUEST);
    }

    void ClientNetwork::LeaveMatch()
    {
        if (matchID_ == 0 || pendingReset_) return;

        switch (netState_)
        {
        case ClientNetworkState::NSS_Hello:
        case ClientNetworkState::NSS_Lobby: // 서버가 내 client를 Matching 중일 때
            SendTCPPacket(TCPPacketType::MATCH_CANCEL);
            break;

        case ClientNetworkState::NSS_Starting:
        case ClientNetworkState::NSS_Playing:
            SendTCPPacket(TCPPacketType::MATCH_LEAVE);
            pendingReset_ = true;
            break;

        default:
            break;
        }
    }

    void ClientNetwork::EndGame()
    {
        if(matchID_ == 0 || pendingReset_) return;
        SendTCPPacket(TCPPacketType::GAME_END);
        pendingReset_ = true;
    }

    // use network thread
    void ClientNetwork::NetThreadLoop()
    {
        SocketAddress fromAddr;
        while (UDPRecvThreadRunning_)
        {
            RawBuffer* buffer = bufferPool_.Allocate();
            int receivedBytes;
            if (driver_.RecvPacket(buffer, receivedBytes, fromAddr))
                pendingPackets_.Emplace(buffer, receivedBytes, fromAddr);
            else
                bufferPool_.Deallocate(buffer);
        }
    }

    void ClientNetwork::ProcessQueuedUDPPacket()
    {
        while (!pendingPackets_.Empty())
        {
            Packet packet;
            pendingPackets_.Dequeue(packet);
            NetBitReader reader(packet.buffer->data(), packet.size);

            uint16_t type;
            reader.ReadInt(type, static_cast<uint16_t>(UDPPacketType::COUNT));

            ProcessUDPPacket(static_cast<UDPPacketType>(type), reader, packet.addr);

            bufferPool_.Deallocate(packet.buffer);
        }
    }

    void ClientNetwork::ProcessUDPPacket(const UDPPacketType type, NetBitReader& reader, const SocketAddress& inToAddress)
    {
        //std::cout << (int)type << '\n';

        switch (type)
        {
        case UDPPacketType::HELLO:
            //Send Welcome to peer
            std::cout << "Receive HELLO\n";
            SendUDPPacket(UDPPacketType::WELCOME, SendPolicy::Unicast, &inToAddress);
            break;

        case UDPPacketType::WELCOME:
            //Check the peer is connected
            std::cout << "Receive WELCOME\n";
            if (netState_ != ClientNetworkState::NSS_Hello) break; // 중복 WELCOME 방지
            if (HandleWelcomePacket(inToAddress))
            {
                std::cout << "All Checked\n";
                SendTCPPacket(TCPPacketType::MATCH_INTRO_SUCCESS);
                netState_ = ClientNetworkState::NSS_Lobby;
            }

            //TODO@ timeout
            break;

        case UDPPacketType::INPUT:
        {
            PlayerId id;
            if (!gameNetSystem_ || !TryGetPeerPlayerID(inToAddress, id)) // reset 후 늦게 온 패킷 무시
                break;

            InputCommand inputCmd;
            inputCmd.Read(reader);
            inputCmd.id = id;
            
            gameNetSystem_->HandleInput(inputCmd);
            break;
        }
        }
    }

    void ClientNetwork::BuildUDPPacketHeader(NetBitWriter& writer, UDPPacketType messageType)
    {
        writer.WriteInt(static_cast<uint16_t>(messageType), static_cast<uint16_t>(UDPPacketType::COUNT));

        switch (messageType)
        {
        case UDPPacketType::HELLO:
            break;

        case UDPPacketType::WELCOME:
            break;

        case UDPPacketType::INPUT:
            break;
        }
    }

    void ClientNetwork::BroadcastToPeers(const void* buffer)
    {
        for (auto it = conn.begin(); it != conn.end(); it++)
        {
            if (netState_ == ClientNetworkState::NSS_Playing && it->second.playerId == playerID_)
                continue;

            if (it->second.state == PeerState::Left || it->second.state == PeerState::Disconnected)
                continue;

            int byteSentCount{};
            driver_.SendPacket(buffer, byteSentCount, it->second.addr);
        }
    }

    void ClientNetwork::SendUDPPacket(const UDPPacketType messageType, SendPolicy policy, const SocketAddress* inToAddress)
    {
        RawBuffer rawBuffer{};
        NetBitWriter writer{ &rawBuffer };

        BuildUDPPacketHeader(writer, messageType);

        NetBitReader reader{ rawBuffer.data(), rawBuffer.size()};
        uint16_t type;
        reader.ReadInt(type, std::numeric_limits<uint16_t>::max());
        //std::cout << type << '\n';
        if (inToAddress)
        {
            int byteSentCount{};
            driver_.SendPacket(rawBuffer.data(), byteSentCount, *inToAddress);
        }

        else
            BroadcastToPeers(rawBuffer.data());
    }

    void ClientNetwork::SendUDPInputPacket(InputCommand& inputCmd)
    {
        RawBuffer rawBuffer{};
        NetBitWriter writer{ &rawBuffer };

        BuildUDPPacketHeader(writer, UDPPacketType::INPUT);
        inputCmd.Write(writer);

        /*NetBitReader reader{ rawBuffer.data(), rawBuffer.size() };
        uint16_t type;
        reader.ReadInt(type, std::numeric_limits<uint16_t>::max());
        std::cout << type << '\n';*/

        BroadcastToPeers(rawBuffer.data());
    }

    bool ClientNetwork::HandleWelcomePacket(const SocketAddress& inToAddress)
    {
        PlayerId id;
        if (!TryGetPeerPlayerID(inToAddress, id) || id >= peerConnected.size())
            return false;

        peerConnected[id] = true;

        for (const auto& peer : peerConnected)
        {
            if (!peer)
                return false;
        }

        return true;
    }

    void ClientNetwork::TCPNetRecvThreadLoop()
    {
        uint8_t segment[MAX_PACKET_SIZE]{};
        while (TCPRecvThreadRunning_)
        {
            if (server_.WaitReable(100) == 0) // 100ms 타임아웃
                continue;

            int received = server_.RecvPacket(segment, MAX_PACKET_SIZE);
            if (received > 0) //parsing
            {
                std::cout << __FUNCTION__<<'\n';
                recvBuffer.insert(recvBuffer.end(), segment, segment + received);

                while (recvBuffer.size() >= sizeof(uint16_t))
                {  
                    uint16_t size;
                    std::memcpy(&size, recvBuffer.data(), sizeof(size));

                    if (size < sizeof(uint16_t)) // TCP 스트림 문제: 패킷 전체 길이 필드(uint16_t)가 깨짐
                    {
                        recvBuffer.clear();
                        break;
                    }
                    if (recvBuffer.size() < size)
                        break;

                    const uint16_t payload = size - sizeof(uint16_t);
                    auto packet = std::make_unique<TCPPacket>(payload, 0);
                    std::memcpy(packet->buffer.data(), recvBuffer.data() + sizeof(uint16_t), payload);
                    
                    recvBuffer.erase(recvBuffer.begin(), recvBuffer.begin() + size);
                    pendingTCPPackets_.Emplace(std::move(packet));
                }
                continue;
            }

            // received <= 0
            if (TCPRecvThreadRunning_) // 실제 서버 끊김
            {
                std::cout << "Disconnected to server.\n";
                connectedToServer = false;
                serverLost_ = true;
            }
            break;
        }
    }

    void ClientNetwork::ProcessQueuedTCPPacket()
    {
        while (!pendingTCPPackets_.Empty())
        {
            std::unique_ptr<TCPPacket> nextPacket;
            pendingTCPPackets_.Dequeue(nextPacket);

            uint16_t type;
            NetBitReader reader(nextPacket->buffer.data(), nextPacket->size());

            reader.ReadInt(type, static_cast<uint16_t>(TCPPacketType::COUNT));

            ProcessTCPPacket(static_cast<TCPPacketType>(type), reader);
        }
    }

    void ClientNetwork::ProcessTCPPacket(const TCPPacketType& header, NetBitReader& reader)
    {
        switch (header)
        {
        case TCPPacketType::MATCH_CANCEL_SUCCESS:
            std::cout << "Success to cancel match.\n";
            pendingReset_ = true;
            break;

        case TCPPacketType::MATCH_CANCEL_FAILED:
            std::cout << "Falied to cancel match.\n";
            if (matchID_ != 0) // 이미 서버에서 게임 시작으로 넘어간 경우
            {
                SendTCPPacket(TCPPacketType::MATCH_LEAVE);
                pendingReset_ = true;
            }
            break;

        case TCPPacketType::MATCH_INTRO:
            netState_ = ClientNetworkState::NSS_Hello;
            //Set NetConnections
            std::cout << "Receive MATCH_INTRO\n";
            HandleMatchIntroPacket(reader);
            //Send Hello Packet to all peers
            SendUDPPacket(UDPPacketType::HELLO, SendPolicy::Broadcast);
            //Send TIME_SYNC_REQ Packet to server
            SendTCPPacket(TCPPacketType::TIME_SYNC_REQ);
            break;

        case TCPPacketType::TIME_SYNC_RES:
            std::cout << "Receive TIME_SYNC_RES\n";
            //Calculate Server Tick
            HandleServerTimeSyncPacket(reader);
            break;

        case TCPPacketType::MATCH_START:
            std::cout << "Receive MATCH_START\n";
            HandleMatchStartPacket(reader);
            break;

        case TCPPacketType::MATCH_PEER_STATUS:
            std::cout << "Receive MATCH_PEER_STATUS\n";
            HandleMatchPeerStatePacket(reader);
            break;
        }
    }

    void ClientNetwork::SendTCPPacket(TCPPacketType messageType)
    {
        if (!connectedToServer) return;
        std::cout << "Send ";

        RawBuffer rawBuffer{};
        NetBitWriter writer{ &rawBuffer };
        writer.WriteInt(static_cast<uint16_t>(0), std::numeric_limits<uint16_t>::max());
        writer.WriteInt(static_cast<uint16_t>(messageType), static_cast<uint16_t>(TCPPacketType::COUNT));

        switch (messageType)
        {
        case TCPPacketType::MATCH_REQUEST:
        {
            // name size + name + udp port number
            writer.WriteInt(static_cast<uint16_t>(name_.size()), std::numeric_limits<uint16_t>::max());
            for (int i = 0; i < name_.size(); i++)
                writer.WriteInt(static_cast<uint8_t>(name_[i]), std::numeric_limits<uint8_t>::max());
            writer.WriteInt(static_cast<uint16_t>(driver_.GetPort()), std::numeric_limits<uint16_t>::max());

            break;
        }

        case TCPPacketType::MATCH_CANCEL:
            std::cout << "MATCH_CANCEL\n";
            break;

        case TCPPacketType::MATCH_INTRO_SUCCESS:
        case TCPPacketType::MATCH_INTRO_FAILED:
        {
            std::cout << "MATCH_INTRO_RESULT: ";
            if (messageType == TCPPacketType::MATCH_INTRO_SUCCESS)
                std::cout << "SUCCESS\n";
            else
                std::cout << "FAILED\n";

            writer.WriteInt(matchID_, std::numeric_limits<MatchId>::max());
            break;
        }

        case TCPPacketType::TIME_SYNC_REQ:
            std::cout << "TIME_SYNC_REQ\n";
            sendTime = static_cast<ServerTimeMs>(
                duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now().time_since_epoch()).count());
            break;

        case TCPPacketType::GAME_END:
        case TCPPacketType::MATCH_LEAVE:
            std::cout << "GAME_END / MATCH_LEAVE\n";
            writer.WriteInt(matchID_, std::numeric_limits<MatchId>::max());
            break;
        }

        uint16_t byteSize = writer.GetByteSize();
        std::memcpy(rawBuffer.data(), &byteSize, sizeof(uint16_t));

        server_.SendPacket(rawBuffer.data(), byteSize);
    }

    void ClientNetwork::HandleMatchIntroPacket(NetBitReader& reader)
    {
        uint8_t readMyId{ 0 }, readPlayerNum{ 0 };
        uint16_t readMatchId{ 0 };

        reader.ReadInt(readMatchId, std::numeric_limits<uint16_t>::max());
        matchID_ = readMatchId;
        std::cout << "[MATCH ID = " << matchID_ << "]\n";

        reader.ReadInt(readPlayerNum, std::numeric_limits<uint8_t>::max());

        /* 이전 매치 데이터가 남지 않도록 초기화 */
        conn.clear();
        addToId.clear();
        peerConnected.assign(readPlayerNum, false);

        for (int i = 0; i < readPlayerNum; i++)
        {
            uint8_t readPlayerId{ 0 }, rname{};
            uint16_t readNameSize{}, readPort{};
            std::string readName{};
            uint32_t addr{ 0 };

            reader.ReadInt(readPlayerId, std::numeric_limits<uint8_t>::max());
            reader.ReadInt(readNameSize, std::numeric_limits<uint16_t>::max());

            for (int j = 0; j < readNameSize; j++)
            {
                reader.ReadInt(rname, std::numeric_limits<uint8_t>::max());
                readName += rname;
            }
            reader.ReadInt(addr, std::numeric_limits<uint32_t>::max());
            reader.ReadInt(readPort, std::numeric_limits<uint16_t>::max());

            tomato::SocketAddress readAddr(addr, readPort);

            conn.try_emplace(readPlayerId, PeerState::Connected, 0, readMatchId, readPlayerId, readName, readAddr);
            addToId.try_emplace(readAddr, readPlayerId);

            std::cout << "[Add Connection]\nmatch id\tplayer id\tname\taddr\n" <<
                '\t' << readMatchId << "\t\t" << int(readPlayerId) << '\t' << readName << '\t' << readAddr.ToString() << '\n';
        }

        reader.ReadInt(readMyId, std::numeric_limits<uint8_t>::max());

        playerID_ = readMyId;
        std::cout << "[My PlayerID = " << int(playerID_) << "]\n";
        
        if(playerID_ < peerConnected.size())
            peerConnected[playerID_] = true;
    }

    void ClientNetwork::HandleServerTimeSyncPacket(NetBitReader& reader)
    {
        recvTime = static_cast<ServerTimeMs>(
            duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count());

        ServerTimeMs serverSteady{};
        reader.ReadInt(serverSteady, std::numeric_limits<ServerTimeMs>::max());

        //@TODO : Calculate server steady time
        ServerTimeMs rtt = recvTime - sendTime;
        serverTimeOffset = serverSteady - (sendTime + (rtt / 2));
    }

    void ClientNetwork::HandleMatchStartPacket(NetBitReader& reader)
    {
        if (netState_ == ClientNetworkState::NSS_Lobby)
        {
            ServerTimeMs serverStartTime{};
            reader.ReadInt(serverStartTime, std::numeric_limits<ServerTimeMs>::max());

            //@TODO : Calculate server tick, inform start tick to engine
            ServerTimeMs localSteadyTimeNow = static_cast<ServerTimeMs>(
                duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now().time_since_epoch()).count());

            ServerTimeMs estimatedServerNow = localSteadyTimeNow + serverTimeOffset;
            ServerTimeMs remaining = serverStartTime - estimatedServerNow;

            //@TODO
            if (remaining < 0)
            {
                std::cout << "Invalid StartTime\n";
                return;
            }
            localStartTime = static_cast<ServerTimeMs>(
                duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now().time_since_epoch()).count()) + remaining;

            //engine_.SetStartTimes(localStartTime);
            std::cout << "[Now] " << localSteadyTimeNow << "\n[Start Time] " << localStartTime << '\n';
            netState_ = ClientNetworkState::NSS_Starting;
        }
    }

    void ClientNetwork::HandleMatchPeerStatePacket(NetBitReader& reader)
    {
        if (netState_ != ClientNetworkState::NSS_Starting &&
            netState_ != ClientNetworkState::NSS_Playing) return;

        uint8_t readTargetPlayerId{ 0 }, readPeerState{ 0 };
        uint16_t readMatchId{ 0 };

        reader.ReadInt(readMatchId, std::numeric_limits<uint16_t>::max());
        if (readMatchId != matchID_)
            return;

        reader.ReadInt(readTargetPlayerId, std::numeric_limits<uint8_t>::max());
        reader.ReadInt(readPeerState, static_cast<uint8_t>(PeerState::COUNT));

        // find taget player's connection
        auto it = conn.find(readTargetPlayerId);
        if (it == conn.end()) return;

        it->second.state = static_cast<PeerState>(readPeerState);
        std::cout << "Player" << (int)readTargetPlayerId << " -> " << ToString(it->second.state) << " [match: " << readMatchId << "]\n";

        PrintNetConnection(); // debug
    }

    void ClientNetwork::ResetMatchState()
    {
        conn.clear();
        addToId.clear();
        peerConnected.clear();
        playerID_ = 0;
        matchID_ = 0;
        localStartTime = 0;
        sendTime = recvTime = 0;
        netState_ = connectedToServer ?
            ClientNetworkState::NSS_Idle : ClientNetworkState::NSS_Uninitialized;
    }

    bool ClientNetwork::TryGetPeerPlayerID(const SocketAddress& addr, PlayerId& out) const
    {
        auto it = addToId.find(addr);
        if (it == addToId.end()) return false;

        out = it->second;

        return true;
    }

    void ClientNetwork::PrintNetConnection()
    {
        std::cout << "\n========== NetConnections ==========\n";

        for (const auto& [playerId, connection] : conn)
        {
            std::cout
                << "[Player ID: " << (int)playerId << "]\n"
                << "  State     : " << ToString(connection.state) << '\n'
                << "  Session ID: " << connection.sessionId << '\n'
                << "  Match ID  : " << connection.matchId << '\n'
                << "  Name      : " << connection.name << '\n'
                << "  Address   : " << connection.addr.ToString() << '\n';
        }

        std::cout << "====================================\n";
    }

    const char* ClientNetwork::ToString(PeerState state)
    {
        switch (state)
        {
        case PeerState::Connected:      return "Connected";
        case PeerState::Finished:       return "Finished";
        case PeerState::Left:           return "Left";
        case PeerState::Disconnected:   return "Disconnected";
        default:                        return "Unknown";
        }
    }

    void ClientNetwork::ThreadStart()
    {
        if (UDPRecvThread.joinable())
            return;

        UDPRecvThreadRunning_ = true;
        UDPRecvThread = std::thread(&ClientNetwork::NetThreadLoop, this);
    }

    void ClientNetwork::ThreadStop()
    {
        UDPRecvThreadRunning_ = false;
        TCPRecvThreadRunning_ = false;

        if (connectedToServer)
            server_.Shutdown(); // 서버가 연결 중일 때 블로킹 recv 깨움

        if(UDPRecvThread.joinable())
            UDPRecvThread.join();

        if(TCPRecvThread.joinable())
            TCPRecvThread.join();

        connectedToServer = false;
    }
}
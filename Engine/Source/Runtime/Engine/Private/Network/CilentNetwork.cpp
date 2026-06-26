#include "Network/ClientNetwork.h"
#include "Network/NetBitReader.h"
#include "Network/NetBitWriter.h"
#include "GameNetwork/Message/InputMessage.h"
#include "Utils/Logger.h"

namespace tomato
{
    ClientNetwork::ClientNetwork()
        : playerID_(0), netState_(ClientNetworkState::NSS_Uninitialized)
    {
        conn.try_emplace((PlayerId)0, NetConnection{ 0, 0, 0, "me", {"192.168.0.105", 9000} });
        conn.try_emplace((PlayerId)1, NetConnection{ 1, 0, 1, "you", {"192.168.0.193", 9000} });
        addToId.try_emplace({ "192.168.0.105", 9000 }, 0);
        addToId.try_emplace({ "192.168.0.193", 9000 }, 1);
    }

    ClientNetwork::~ClientNetwork() {}

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
            InputCommand inputCmd;
            inputCmd.Read(reader);
            inputCmd.id = GetPeerPlayerID(inToAddress);
            
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
            if (it->second.playerId == playerID_)
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
        PlayerId id = GetPeerPlayerID(inToAddress);
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
            int received = server_.RecvPacket(segment, MAX_PACKET_SIZE);
            if (received > 0) //parsing
            {
                recvBuffer.insert(recvBuffer.end(), segment, segment + received);
                if (recvBuffer.size() < sizeof(uint16_t))
                    continue;

                uint16_t size;
                std::memcpy(&size, recvBuffer.data(), sizeof(size));

                if (recvBuffer.size() < size)
                    continue;

                auto packet = std::make_unique<TCPPacket>(size, 0);
                std::memcpy(packet->buffer.data(), recvBuffer.data() + sizeof(uint16_t), size);

                recvBuffer.erase(recvBuffer.begin(), recvBuffer.begin() + size);

                pendingTCPPackets_.Emplace(std::move(packet));
            }
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
        }
    }

    void ClientNetwork::SendTCPPacket(TCPPacketType messageType)
    {
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
        case TCPPacketType::MATCH_INTRO_SUCCESS:
        case TCPPacketType::MATCH_INTRO_FAILED:
        {
            std::cout << "MATCH_INTRO_SUCCESS\n";
            writer.WriteInt(matchID_, std::numeric_limits<MatchId>::max());
            break;
        }

        case TCPPacketType::TIME_SYNC_REQ:
            std::cout << "MATCH_INTRO_SUCCESS\n";
            sendTime = static_cast<ServerTimeMs>(
                duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now().time_since_epoch()).count());
            break;
        }

        int16_t byteSize = writer.GetByteSize();
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
        peerConnected.resize(readPlayerNum);

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

            conn.try_emplace(readPlayerId, 0, readMatchId, readPlayerId, readName, readAddr);
            addToId.try_emplace(readAddr, readPlayerId);

            std::cout << "[Add Connection]\nmatch id\tplayer id\tname\taddr\n" <<
                '\t' << readMatchId << '\t' << int(readPlayerId) << '\t' << readName << '\t' << readAddr.ToString() << '\n';
        }

        reader.ReadInt(readMyId, std::numeric_limits<uint8_t>::max());

        playerID_ = readMyId;
        std::cout << "[My PlayerID = " << int(playerID_) << "]\n";
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

        //estimatedServerTime = localSteadyNow + offset
        /*ServerTimeMs estimatedServerTime = static_cast<ServerTimeMs>(
            duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count()) + serverTimeOffset;*/

                /*estimatedServerTick = floor(estimatedServerTime / 16.67f);
                engine_.SetServerTicks(estimatedServerTick);*/

                //std::cout << "[Server Tick] " << int(estimatedServerTick) << "\n[Local Tick] " << int(engine_.GetTick()) << '\n';
    }

    void ClientNetwork::HandleMatchStartPacket(NetBitReader& reader)
    {
        if (netState_ == ClientNetworkState::NSS_Lobby)
        {
            ServerTimeMs serverStartTime{};
            reader.ReadInt(serverStartTime, std::numeric_limits<ServerTimeMs>::max());

            /*estimatedStartTick = floor(serverStartTime / 16.67f );
            engine_.SetStartTicks(estimatedStartTick);*/
            //std::cout << "[Start Tick] " << int(estimatedStartTick) << '\n';

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

    PlayerId ClientNetwork::GetPeerPlayerID(const SocketAddress& addr)
    {
        auto it = addToId.find(addr);
        if (it == addToId.end())
            throw std::runtime_error("address not found");

        return it->second;
    }

    void ClientNetwork::ThreadStart()
    {
        UDPRecvThread = std::thread(&ClientNetwork::NetThreadLoop, this);
        UDPRecvThreadRunning_ = true;

        if (server_.IsConnectedToServer())
        {
            TCPRecvThread = std::thread(&ClientNetwork::TCPNetRecvThreadLoop, this);
            TCPRecvThreadRunning_ = true;
        }
    }

    void ClientNetwork::ThreadStop()
    {
        UDPRecvThreadRunning_ = false;
        UDPRecvThread.join();

        if (server_.IsConnectedToServer())
        {
            TCPRecvThreadRunning_ = false;
            TCPRecvThread.join();
        }
    }
}
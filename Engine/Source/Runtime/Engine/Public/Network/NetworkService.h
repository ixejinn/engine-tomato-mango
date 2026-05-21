#ifndef MANGO_NETWORKSERVICE_H
#define MANGO_NETWORKSERVICE_H

#include <bitset>
#include <map>
#include <unordered_map>
#include <string>
#include <atomic>
#include <thread>

#include "../../Core/Private/Containers/MemoryPool.h"
#include "../../Core/Private/Containers/SPSCQueue.h"

#include "Network/NetTypes.h"
#include "Network/NetworkFwd.h"
#include "Network/WinsockInitializer.h"
#include "Network/UDPNetDriver.h"
#include "Network/TCPNetDriver.h"
#include "Network/NetConnection.h"

namespace tomato
{
    class Engine;

    // Represents current network state of the client during match flow.
    enum class NetworkServiceState : uint8_t
    {
        NSS_Uninitialized,
        NSS_Hello,          // Received MATCH_INTRO, attempting peer connections
        NSS_Lobby,          // All peers connected, waiting for match start
        NSS_Starting,       // Received MATCH_START, waiting for start tick
        NSS_Playing,        // Match in progress
    };

    class NetworkService
    {
    public:
        explicit NetworkService(NetMode mode);
        ~NetworkService();

        void SetNetState(NetworkServiceState state) { netState_ = state; }

        void NetThreadLoop();
        void ProcessQueuedUDPPacket();
        void ProcessUDPPacket(const UDPPacketType& type, NetBitReader& reader, const SocketAddress& inToAddress);

        void BuildUDPPacket(NetBitWriter& writer, UDPPacketType messageType);
        void BroadcastToPeers(const void* buffer);
        void SendUDPPacket(UDPPacketType messageType, SendPolicy policy, const SocketAddress* inToAddress = nullptr);

        bool HandleWelcomePacket(const SocketAddress& inToAddress);

        void TCPNetRecvThreadLoop();
        void ProcessQueuedTCPPacket();
        void ProcessTCPPacket(const TCPPacketType& header, NetBitReader& reader);
        void SendTCPPacket(TCPPacketType messageType);

        void HandleMatchIntroPacket(NetBitReader& reader);
        void HandleServerTimeSyncPacket(NetBitReader& reader);
        void HandleMatchStartPacket(NetBitReader& reader);

        void ThreadStart();
        void ThreadStop();

        PlayerId GetMyPlayerID() const { return playerID_; }
        PlayerId GetPeerPlayerID(const SocketAddress& addr);

        NetworkServiceState GetNetState() const { return netState_; }
        ServerTimeMs GetLocalStartTime() const { return localStartTime; }

    private:
        WinsockInitializer winsockInit_;
        UDPNetDriver driver_;
        TCPNetDriver server_;

        std::thread TCPRecvThread, UDPRecvThread;
        bool TCPRecvThreadRunning_{ false }, UDPRecvThreadRunning_{ false };

        std::vector<uint8_t> recvBuffer;
        SPSCQueue<std::unique_ptr<TCPPacket>, 1024> pendingTCPPackets_;

        MemoryPool<RawBuffer, 1024> bufferPool_;
        SPSCQueue<Packet, 1024> pendingPackets_;

        std::vector<bool> peerConnected;
        std::unordered_map<PlayerId, NetConnection> conn;
        std::unordered_map<SocketAddress, PlayerId> addToId;

        NetworkServiceState netState_;
        std::string name_ = "testing";
        PlayerId playerID_{ 0 };
        MatchId matchID_{ 0 };

        ServerTimeMs sendTime{ 0 }, recvTime{ 0 };
        ServerTimeMs serverTimeOffset{ 0 }, localStartTime{ 0 };
    };
}

#endif // !MANGO_NETWORKSERVICE_H
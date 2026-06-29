#ifndef MANGO_CLIENTNETWORK_H
#define MANGO_CLIENTNETWORK_H

#include <bitset>
#include <map>
#include <unordered_map>
#include <string>
#include <atomic>
#include <thread>

#include "Containers/MemoryPool.h"
#include "Containers/SPSCQueue.h"

#include "ECS/Forward/SystemFwd.h"
#include "Network/NetTypes.h"
#include "Network/NetworkFwd.h"
#include "Network/WinsockInitializer.h"
#include "Network/UDPNetDriver.h"
#include "Network/TCPNetDriver.h"
#include "Network/NetConnection.h"
#include "GameNetwork/GamePlayNetSystem.h"

namespace tomato
{
    class Engine;

    // Represents current network state of the client during match flow.
    enum class ClientNetworkState : uint8_t
    {
        NSS_Uninitialized,
        NSS_Hello,          // Received MATCH_INTRO, attempting peer connections
        NSS_Lobby,          // All peers connected, waiting for match start
        NSS_Starting,       // Received MATCH_START, waiting for start tick
        NSS_Playing,        // Match in progress
    };

    class ClientNetwork
    {
    public:
        explicit ClientNetwork();
        ~ClientNetwork();

        void SetNetState(ClientNetworkState state) { netState_ = state; }

        void NetThreadLoop();
        void ProcessQueuedUDPPacket();
        void ProcessUDPPacket(const UDPPacketType type, NetBitReader& reader, const SocketAddress& inToAddress);

        void BuildUDPPacketHeader(NetBitWriter& writer, UDPPacketType messageType);
        void BroadcastToPeers(const void* buffer);
        void SendUDPPacket(const UDPPacketType messageType, SendPolicy policy, const SocketAddress* inToAddress = nullptr);
        void SendUDPInputPacket(InputCommand& inputCmd);
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

        ClientNetworkState GetNetState() const { return netState_; }
        ServerTimeMs GetLocalStartTime() const { return localStartTime; }

        void SetGameplaySystem(GamePlayNetSystem* gameNetSys) { gameNetSystem_ = gameNetSys; }

    private:
        WinsockInitializer winsockInit_;
        UDPNetDriver driver_;
        TCPNetDriver server_;

        std::thread TCPRecvThread, UDPRecvThread;
        bool TCPRecvThreadRunning_{ false }, UDPRecvThreadRunning_{ false };

        std::vector<uint8_t> recvBuffer;
        SPSCQueue<std::unique_ptr<TCPPacket>, 256> pendingTCPPackets_;

        MemoryPool<RawBuffer, 256> bufferPool_;
        SPSCQueue<Packet, 256> pendingPackets_;

        std::vector<bool> peerConnected;
        std::unordered_map<PlayerId, NetConnection> conn;
        std::unordered_map<SocketAddress, PlayerId> addToId;

        ClientNetworkState netState_;
        std::string name_ = "testing";
        PlayerId playerID_;
        MatchId matchID_{ 0 };

        ServerTimeMs sendTime{ 0 }, recvTime{ 0 };
        ServerTimeMs serverTimeOffset{ 0 }, localStartTime{ 0 };

        GamePlayNetSystem* gameNetSystem_{ nullptr };
    };
}

#endif // !MANGO_CLIENTNETWORK_H
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
#include "GameNetwork/NetInputSync.h"

namespace tomato
{
    class Engine;

    // Represents current network state of the client during match flow.
    enum class ClientNetworkState : uint8_t
    {
        NSS_Uninitialized,  // 서버 미연결
        NSS_Idle,           // 서버 연결됨, 매치 없음
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

        void Update();

        void ConnectToServer();
        void RequestMatch();
        void LeaveMatch();
        void EndGame();

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
        void HandleMatchPeerStatePacket(NetBitReader& reader);

        void ThreadStart();
        void ThreadStop();

        PlayerId GetMyPlayerID() const { return playerID_; }
        PlayerId GetPeerPlayerID(const SocketAddress& addr); // no use

        ClientNetworkState GetNetState() const { return netState_; }
        ServerTimeMs GetLocalStartTime() const { return localStartTime; }
        bool IsPlaying() const { return netState_ == ClientNetworkState::NSS_Playing && !pendingReset_; }

        void SetGameplaySystem(NetInputSync* gameNetSys) { gameNetSystem_ = gameNetSys; }

    private:
        void ResetMatchState();
        bool TryGetPeerPlayerID(const SocketAddress& addr, PlayerId& out) const;

        void PrintNetConnection(); // debug
        const char* ToString(PeerState state);

    private:
        WinsockInitializer winsockInit_;
        UDPNetDriver driver_;
        TCPNetDriver server_;
        bool connectedToServer{ false };

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

        bool pendingReset_{ false };
        std::atomic<bool> serverLost_{ false }; // recv 스레드

        ServerTimeMs sendTime{ 0 }, recvTime{ 0 };
        ServerTimeMs serverTimeOffset{ 0 }, localStartTime{ 0 };

        NetInputSync* gameNetSystem_{ nullptr };
    };
}

#endif // !MANGO_CLIENTNETWORK_H
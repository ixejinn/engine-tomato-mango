#ifndef MANGO_MATCHSERVERNETWORK_H
#define MANGO_MATCHSERVERNETWORK_H

#include <thread>

#include "Containers/SPSCQueue.h"

#include "Network/WinsockInitializer.h"
#include "Network/TCPNetDriver.h"

#include "Network/PacketTypes.h"
#include "MatchTypes.h"

namespace tomato
{
	class NetBitReader;
	class NetBitWriter;

	class SessionManager;
	class MatchManager;

	class MatchServerNetwork
	{
	public:
		MatchServerNetwork(SessionManager& sessionMgr, MatchManager& matchMgr, SPSCQueue<MatchRequestCommand, 128>& requestQ, SPSCQueue<SendCommandPtr, 256>& netSendRequestQ);

		void Update();

		void ProcessQueuedPackets();
		void ProcessPacket(const TCPPacketType& header, NetBitReader& reader, SessionId& client);
		void HandlePacketRequest(const TCPPacketType& header, NetBitReader& reader, SessionId& client);
		void HandlePacketTimeSync(const TCPPacketType& header, NetBitReader& reader, SessionId& client);

		void ProcessNetSendRequest();
		void ProcessSendPacket();

		void ProcessDataFromClient(const SessionId& sessionId, const uint8_t* data, const int len);

		void TCPRecvThreadLoop();
		void ThreadStart();
		void ThreadStop();

	private:
		WinsockInitializer winsockInit_;
		TCPNetDriver tcpDriver_;

		SessionManager& sessionMgr_;
		MatchManager& matchMgr_;

		SPSCQueue<TCPPacketPtr, 256> TCPRecvQueue;
		SPSCQueue<MatchRequestCommand, 128>& MatchRequestQueue;
		SPSCQueue<SendCommandPtr, 256>& NetSendRequestQueue;

		std::thread recvThread;
		bool isThreadRunning = false;

		MatchId testMatchId = 0;
	};
}

#endif // !MANGO_MATCHSERVERNETWORK_H

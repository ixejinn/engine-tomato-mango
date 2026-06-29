#ifndef MANGO_MATCHSERVER_H
#define MANGO_MATCHSERVER_H

#include <thread>
#include <chrono>

#include "Containers/SPSCQueue.h"
#include "SessionManager.h"
#include "MatchManager.h"
#include "MatchServerNetwork.h"

namespace tomato
{
	class MatchServer
	{
	public:
		MatchServer();
		void Run();

	private:
		bool isRunning_{ true };

		SessionManager sessionMgr_;
		MatchManager matchMgr_;
		MatchServerNetwork serverNetwork_;

		SPSCQueue<SendCommandPtr, 256> NetSendRequestQueue;
		SPSCQueue<MatchRequestCommand, 128> MatchRequestQueue;
	};
}

#endif // !MANGO_MATCHSERVER_H

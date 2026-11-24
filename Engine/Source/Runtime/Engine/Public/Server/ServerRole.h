#ifndef MANGO_MATCHSERVER_H
#define MANGO_MATCHSERVER_H

#include <thread>
#include <atomic>
#include <chrono>

#include "Containers/SPSCQueue.h"
#include "SessionManager.h"
#include "MatchManager.h"
#include "MatchServerNetwork.h"

namespace tomato
{
	class ServerRole
	{
	public:
		ServerRole();
		ServerRole(const ServerRole&) = delete;
		ServerRole& operator=(const ServerRole&) = delete;
		~ServerRole() { Stop(); }

		bool Start();
		void Stop();
		bool IsRunning() const { return isRunning_; }

	private:
		void Loop();

	private:
		SPSCQueue<MatchRequestCommand, 128> MatchRequestQueue;
		SPSCQueue<SendCommandPtr, 256> NetSendRequestQueue;

		SessionManager sessionMgr_;
		MatchManager matchMgr_;
		MatchServerNetwork serverNetwork_;


		std::atomic<bool> isRunning_{ false };
		std::thread serverThread_;
	};
}

#endif // !MANGO_MATCHSERVER_H

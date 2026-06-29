#include "Server/MatchServer.h"

namespace tomato
{
	MatchServer::MatchServer()
		:
		matchMgr_(MatchRequestQueue, NetSendRequestQueue),
		serverNetwork_(sessionMgr_, matchMgr_, MatchRequestQueue, NetSendRequestQueue) {};

	void MatchServer::Run()
	{
		constexpr float fixedDt = 1.0f / 60.0f;

		float accumulator = 0.0f;
		auto prev = std::chrono::steady_clock::now();

		serverNetwork_.ThreadStart();
		while (isRunning_)
		{
			auto now = std::chrono::steady_clock::now();
			std::chrono::duration<float> delta = now - prev;
			prev = now;

			accumulator += delta.count();

			while (accumulator >= fixedDt)
			{
				serverNetwork_.Update();
				matchMgr_.Update(fixedDt);

				accumulator -= fixedDt;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		serverNetwork_.ThreadStop();
	};
}
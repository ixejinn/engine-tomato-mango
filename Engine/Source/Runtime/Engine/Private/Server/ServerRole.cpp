#include "Server/ServerRole.h"

namespace tomato
{
	ServerRole::ServerRole()
		:
		matchMgr_(MatchRequestQueue, NetSendRequestQueue),
		serverNetwork_(sessionMgr_, matchMgr_, MatchRequestQueue, NetSendRequestQueue) {};

	bool ServerRole::Start()
	{
		if (serverThread_.joinable())
			return true;

		if (!serverNetwork_.IsListening())
			return false;

		isRunning_ = true;
		serverNetwork_.ThreadStart();
		serverThread_ = std::thread(&ServerRole::Loop, this);

		return true;
	}

	void ServerRole::Stop()
	{
		isRunning_ = false;
		if (serverThread_.joinable())
			serverThread_.join();

		serverNetwork_.ThreadStop();
	}

	void ServerRole::Loop()
	{
		constexpr float fixedDt = 1.0f / 60.0f;

		float accumulator = 0.0f;
		auto prev = std::chrono::steady_clock::now();

		try
		{
			while (isRunning_)
			{
				auto now = std::chrono::steady_clock::now();
				accumulator += std::chrono::duration<float>(now - prev).count();
				prev = now;

				if (accumulator > 0.25f)
					accumulator = 0.25f;

				while (accumulator >= fixedDt)
				{
					serverNetwork_.Update();
					matchMgr_.Update(fixedDt);

					accumulator -= fixedDt;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}
		catch (const std::exception& e)
		{
			std::cout << "[ServerRole] exception: " << e.what() << '\n';
			isRunning_ = false;
		}
	}
}
#ifndef MANGO_NETWORKRUNTIME_H
#define MANGO_NETWORKRUNTIME_H

#include <memory>

#include "Simulation/Tick/TickFwd.h"
#include "NetTypes.h"
#include "ClientRole.h"

namespace tomato
{
	class NetworkRuntime
	{
	public:
		void EnableClient() { client_ = std::make_unique<ClientRole>(); }
		void DiableClient() { client_.reset(); }

		void Start()	{ if(client_) client_->Start(); }
		void Stop()		{ if(client_) client_->Stop(); }
		void PollIncoming(TickClock& tc)
		{
			if (client_) client_->PollIncoming(tc);
		}

		bool ShoudRollback(uint64_t currT, int64_t& outLateT);
		void SetMode(NetMode mode) { netMode_ = mode; }
		NetMode GetMode() const { return netMode_; }

		ClientRole& Client() { return *client_; }

	private:
		NetMode netMode_;
		std::unique_ptr<ClientRole> client_{ nullptr };
	};
}

#endif // !MANGO_NETWORKRUNTIME_H

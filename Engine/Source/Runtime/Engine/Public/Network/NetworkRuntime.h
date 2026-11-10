#ifndef MANGO_NETWORKRUNTIME_H
#define MANGO_NETWORKRUNTIME_H

#include <memory>
#include <optional>

#include "Simulation/Tick/TickFwd.h"
#include "NetTypes.h"
#include "ClientRole.h"

namespace tomato
{
	class NetworkRuntime
	{
	public:
		void RequestClient(bool enable) { pendingClient_ = enable; } // 요청 저장
		void EnableClient(State* currentState);
		void DisableClient() { client_.reset(); client_ = nullptr; }

		void ApplyPending(State* currentState);

		void Start()	{ if(client_) client_->Start(); }
		void Stop()		{ if(client_) client_->Stop(); }
		void PollIncoming(TickClock& tc)
		{
			if (client_) client_->PollIncoming(tc);
		}

		PlayerId GetMyPlayerID();
		void ProcessOutgoingMessages(int64_t tick);
		void OnStateChanged(State* state, SimContext& simCtx);
		bool ShouldRollback(uint64_t currT, int64_t& outLateT);
		void CaptureSnapshot(SimContext& ctx);
		void RollBack(SimContext& ctx);

		void SetMode(NetMode mode) { netMode_ = mode; }
		NetMode GetMode() const { return netMode_; }

		bool HasClient() { return client_ ? true : false; }
		ClientRole& Client() { return *client_; }
	private:
		NetMode netMode_;
		std::optional<bool> pendingClient_;
		//std::optional<bool> pendingServer_;

		std::unique_ptr<ClientRole> client_{ nullptr };
	};
}

#endif // !MANGO_NETWORKRUNTIME_H

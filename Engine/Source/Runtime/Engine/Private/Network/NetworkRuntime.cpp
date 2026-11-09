#include "Network/NetworkRuntime.h"

namespace tomato
{
	void NetworkRuntime::EnableClient(State* currentState)
	{
		client_ = std::make_unique<ClientRole>();
		client_->GameNet().SetState(currentState);
		client_->Start();
	}

	void NetworkRuntime::ApplyPending(State* currentState)
	{
		if (!currentState) return;
		if (!pendingClient_) return; // 요청 없음
		
		bool enable = *pendingClient_;
		pendingClient_.reset(); // 중복 방지

		if (enable && !client_)
		{
			EnableClient(currentState);
			std::cout << "Enable Client applied\n";
		}
		else if (!enable && client_)
		{
			DisableClient();
			std::cout << "Disable Client applied\n";
		}
	}

	PlayerId NetworkRuntime::GetMyPlayerID()
	{
		return client_ ? client_->Network().GetMyPlayerID() : 0;
	}

	void NetworkRuntime::ProcessOutgoingMessages(int64_t tick)
	{
		if (!client_) return;
		if (client_->Network().GetNetState() == ClientNetworkState::NSS_Playing)
			client_->GameNet().ProcessOutgoingMessages(tick);
	}

	void NetworkRuntime::OnStateChanged(State* state, SimContext& simCtx)
	{
		if (!client_) return;
		client_->GameNet().SetState(state);
		client_->Rollback().Capture(simCtx);
	}

	bool NetworkRuntime::ShouldRollback(uint64_t currT, int64_t& outLateT)
	{
		if (!client_) return false; // alone 모드일 때

		auto lateT = client_->GameNet().GetConfirmedTick();
		if (currT > lateT && currT - lateT <= ROLLBACK_WINDOW)
		{
			outLateT = lateT;
			return true;
		}
		return false;
	}

	void NetworkRuntime::CaptureSnapshot(SimContext& ctx)
	{
		if (!client_) return;
		client_->Rollback().Capture(ctx);
	}

	void NetworkRuntime::RollBack(SimContext& ctx)
	{
		if (!client_) return;
		client_->Rollback().Rollback(ctx);
	}
}
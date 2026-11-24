#include "Network/NetworkRuntime.h"

namespace tomato
{
	void NetworkRuntime::EnableClient(State* currentState)
	{
		client_ = std::make_unique<ClientRole>();
		client_->GameNet().SetState(currentState);
		client_->Start();

		if (!client_->Network().ConnectToServer())
		{
			std::cout << "Can not Connect Server. Please check if the server is active and try again.\n";
			return;
		}
		//client_->Network().RequestMatch();
	}

	void NetworkRuntime::EnableServer()
	{
		auto server = std::make_unique<ServerRole>();
		if (!server->Start())
		{
			std::cout << "Can not start server. Is the port already in use?\n";
			return;
		}
		server_ = std::move(server);
	}

	void NetworkRuntime::ApplyPending(State* currentState)
	{
		if (!currentState) return;

		// 끌 때: 클라이언트 -> 서버, 켤 때: 서버 -> 클라이언트
		if (pendingClient_ && !*pendingClient_)
		{
			pendingClient_.reset();
			if (client_)
			{
				DisableClient();
				std::cout << "Disable Client applied.\n";
			}
		}
		
		if (pendingServer_)
		{
			const bool enable = *pendingServer_;
			pendingServer_.reset(); // 중복 방지
			
			if (enable && !server_)
			{
				EnableServer();
				std::cout << "Enable Server applied\n";
			}
			else if (!enable && server_)
			{
				DisableServer();
				std::cout << "Disable Server applied\n";
			}
		}

		if (pendingClient_ && *pendingClient_)
		{
			pendingClient_.reset();
			if (!client_)
			{
				EnableClient(currentState);
				std::cout << "Enable Client applied\n";
			}
		}
	}

	PlayerId NetworkRuntime::GetMyPlayerID()
	{
		return client_ ? client_->Network().GetMyPlayerID() : 0;
	}

	void NetworkRuntime::ProcessOutgoingMessages(int64_t tick)
	{
		if (!client_) return;
		if (client_->Network().IsPlaying())
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
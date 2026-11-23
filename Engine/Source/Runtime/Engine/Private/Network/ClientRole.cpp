#include "Network/ClientRole.h"
#include "Simulation/Tick/TickClock.h"
#include <chrono>
namespace tomato
{
	ClientRole::ClientRole() : gameNet_(nullptr)
	{
		network_.SetGameplaySystem(&gameNet_);
		gameNet_.SetNetwork(&network_);
	}

	ClientRole::~ClientRole()
	{
		if (network_.GetNetState() == ClientNetworkState::NSS_Starting || network_.IsPlaying())
			network_.LeaveMatch();

		network_.ThreadStop(); // 스레드 정리 후 나머지 멤버 파괴
	}

	bool ClientRole::PollIncoming(TickClock& tc)
	{
		gameNet_.InitializeConfirmedTick(tc.GetTick());
		return network_.Update();
	}
}
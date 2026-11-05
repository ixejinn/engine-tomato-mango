#include "Network/ClientRole.h"
#include "Simulation/Tick/TickClock.h"

namespace tomato
{
	ClientRole::ClientRole() : gameNet_(nullptr)
	{
		network_.SetGameplaySystem(&gameNet_);
		gameNet_.SetNetwork(&network_);
	}

	ClientRole::~ClientRole()
	{
		network_.ThreadStop(); // 스레드 정리 후 나머지 멤버 파괴
	}

	void ClientRole::PollIncoming(TickClock& tc)
	{
		gameNet_.InitializeConfirmedTick(tc.GetTick());
		network_.ProcessQueuedUDPPacket();
		network_.ProcessQueuedTCPPacket();
	}
}
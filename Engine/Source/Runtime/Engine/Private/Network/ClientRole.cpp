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
		//auto start = std::chrono::high_resolution_clock::now();
		network_.ThreadStop(); // 스레드 정리 후 나머지 멤버 파괴
		//auto end = std::chrono::high_resolution_clock::now();
		//std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "\n";
	}


	void ClientRole::PollIncoming(TickClock& tc)
	{
		gameNet_.InitializeConfirmedTick(tc.GetTick());
		network_.ProcessQueuedUDPPacket();
		network_.ProcessQueuedTCPPacket();
	}
}
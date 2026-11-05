#ifndef MANGO_CLIENTROLE_H
#define MANGO_CLIENTROLE_H

#include "Simulation/Tick/TickFwd.h"
#include "Network/ClientNetwork.h"
#include "GameNetwork/GamePlayNetSystem.h"
#include "GameNetwork/Rollback/RollbackManager.h"

namespace tomato
{
	class ClientRole
	{
	public:
		ClientRole();
		ClientRole(const ClientRole&) = delete;
		ClientRole& operator=(const ClientRole&) = delete;
		~ClientRole();

		void Start() { network_.ThreadStart(); }
		void Stop() { network_.ThreadStop(); }
		void PollIncoming(TickClock& tc);

		ClientNetwork& Network() { return network_; }
		GamePlayNetSystem& GameNet() { return gameNet_; }
		RollbackManager& Rollback() { return rollbackManager_; }

	private:
		ClientNetwork		network_;
		GamePlayNetSystem	gameNet_;
		RollbackManager		rollbackManager_;
	};
}

#endif // !MANGO_CLIENTROLE_H

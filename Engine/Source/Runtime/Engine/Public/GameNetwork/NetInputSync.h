#ifndef MANGO_GAMEPLAYNETSYSTEM_H
#define MANGO_GAMEPLAYNETSYSTEM_H

#include "Network/NetworkFwd.h"
#include "GameNetwork/GameNetworkFwd.h"
#include "State/StateFwd.h"

namespace tomato
{
	class NetInputSync
	{
	public:
		NetInputSync(State* state);

		void OnInputReceived(const InputCommand& inputCmd);

		void SendLocalInput(uint32_t tick);

		void OnMatchEnded() { currentStatePtr_ = nullptr; }

		void SetState(State* newState) { currentStatePtr_ = newState; }
		void SetNetwork(ClientNetwork* network) { network_ = network; }

        void ResetRollbackTick(uint32_t t) { rollbackTick_ = t; }
        int64_t GetRollbackTick() const { return rollbackTick_; }

	private:
		State* currentStatePtr_{ nullptr };
		ClientNetwork* network_{ nullptr };

        int64_t rollbackTick_{0};
	};
}
#endif // !MANGO_GAMEPLAYNETSYSTEM_H

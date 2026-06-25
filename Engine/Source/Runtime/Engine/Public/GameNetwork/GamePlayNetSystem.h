#ifndef MANGO_GAMEPLAYNETSYSTEM_H
#define MANGO_GAMEPLAYNETSYSTEM_H

#include "../../../Core/Public/Containers/SPSCQueue.h"
#include "Network/NetworkFwd.h"
#include "State/StateFwd.h"

namespace tomato
{
	struct NetMessage;
	struct InputCommand;

	//Create, Destroy Entity
	class GamePlayNetSystem
	{
	public:
		GamePlayNetSystem(State* state);

		void HandleInput(const InputCommand& inputCmd);
		//void HandleMessage(std::unique_ptr<NetMessage> message);

		void ProcessOutgoingMessages(uint32_t tick);

		void SetState(State* newState) { currentStatePtr_ = newState; }
		void SetNetwork(ClientNetwork* network) { network_ = network; }

        void ResetLatestTick(uint32_t t) { latestTick_ = t; }
        uint32_t GetLatestTick() const { return latestTick_; }

	private:
		State* currentStatePtr_{ nullptr };
		ClientNetwork* network_;

        uint32_t latestTick_{0};
	};
}
#endif // !MANGO_GAMEPLAYNETSYSTEM_H

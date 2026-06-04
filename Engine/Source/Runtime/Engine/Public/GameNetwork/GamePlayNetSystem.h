#ifndef MANGO_GAMEPLAYNETSYSTEM_H
#define MANGO_GAMEPLAYNETSYSTEM_H

#include "../../Core/Private/Containers/SPSCQueue.h"
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

	private:
		State* currentStatePtr_{ nullptr };
		ClientNetwork* network_;
	};
}
#endif // !MANGO_GAMEPLAYNETSYSTEM_H

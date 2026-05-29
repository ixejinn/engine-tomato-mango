#ifndef MANGO_GAMEPLAYNETSYSTEM_H
#define MANGO_GAMEPLAYNETSYSTEM_H

#include "Network/NetworkFwd.h"
#include "State/StateFwd.h"
#include "Containers/SPSCQueue.h"

namespace tomato
{
	struct NetMessage;
	struct InputCommand;

	//Create, Destroy Entity
	class GamePlayNetSystem
	{
	public:
		GamePlayNetSystem(State* state, ClientNetwork* network, SPSCQueue<InputCommand, 256>& inputCmdQueue, SPSCQueue<NetMessage*, 256>& dataQueue);

		void BeginFrame(); // Start Sync
		void EndFrame();	// End Serialize

		void SetState(State* newState) { currentStatePtr_ = newState; }

	private:
		State* currentStatePtr_{ nullptr };
		ClientNetwork* network_;

		SPSCQueue<InputCommand, 256>& inputCmdQueue;
		SPSCQueue<NetMessage*, 256>& gameDataQueue;
	};
}
#endif // !MANGO_GAMEPLAYNETSYSTEM_H

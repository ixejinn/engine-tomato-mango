#include "GameNetwork/GamePlayNetSystem.h"
#include "Network/ClientNetwork.h"

#include "GameNetwork/Message/NetMessage.h"
#include "GameNetwork/Message/InputMessage.h"

#include "State/State.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"

namespace tomato
{
	NetInputSync::NetInputSync(State* state) : currentStatePtr_(state) {}

	void NetInputSync::OnInputReceived(const InputCommand& inputCmd)
	{
		currentStatePtr_->SetPlayerInput(inputCmd.snapshot.tick, inputCmd.snapshot, inputCmd.id);
        rollbackTick_ = std::min(rollbackTick_, inputCmd.snapshot.tick);
	}

	void NetInputSync::SendLocalInput(uint32_t tick)
	{
		if (!network_)
			return;

		PlayerId myId = network_->GetMyPlayerID();

		auto currInput = currentStatePtr_->GetPlayerInputTimelines()[myId][tick];

		InputCommand inputCmd{myId, currInput};

		if(currInput.tick == tick)
			network_->SendUDPInputPacket(inputCmd);
	}
}

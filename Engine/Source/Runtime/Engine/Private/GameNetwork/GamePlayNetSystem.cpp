#include "GameNetwork/GamePlayNetSystem.h"
#include "Network/ClientNetwork.h"

#include "GameNetwork/Message/NetMessage.h"
#include "GameNetwork/Message/InputMessage.h"

#include "State/State.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"

namespace tomato
{
	GamePlayNetSystem::GamePlayNetSystem(State* state) : currentStatePtr_(state) {}

	void GamePlayNetSystem::HandleInput(const InputCommand& inputCmd)
	{
		currentStatePtr_->SetPlayerInput(inputCmd.snapshot.tick, inputCmd.snapshot, inputCmd.id);
        confirmedTick = std::min(confirmedTick, inputCmd.snapshot.tick);
	}

	void GamePlayNetSystem::ProcessOutgoingMessages(uint32_t tick)
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

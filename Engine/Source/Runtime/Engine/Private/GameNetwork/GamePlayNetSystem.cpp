#include "GameNetwork/GamePlayNetSystem.h"
#include "Network/ClientNetwork.h"

#include "GameNetwork/Message/NetMessage.h"
#include "GameNetwork/Message/InputMessage.h"

#include "State/State.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"

namespace tomato
{
	GamePlayNetSystem::GamePlayNetSystem(State* state)
		: currentStatePtr_(state)
	{
	}

	void GamePlayNetSystem::HandleInput(const InputCommand& inputCmd)
	{
		currentStatePtr_->SetPlayerInput(inputCmd.snapshot.tick, inputCmd.snapshot, inputCmd.id);
        confirmedTick = std::min(confirmedTick, inputCmd.snapshot.tick);

		// std::cout << "GamePlayNetSystem::HandleInput " << inputCmd.snapshot.tick << " " << (int)inputCmd.snapshot.down << " " << (int)inputCmd.snapshot.held << '\n';
		//currentStatePtr_->GetPlayerInputTimelines()[inputCmd.id][inputCmd.snapshot.tick] = inputCmd.snapshot;
		//std::cout << "[ INPUT ] " << inputCmd.snapshot.tick << " " << (int)inputCmd.snapshot.down << " " << (int)inputCmd.snapshot.held << '\n';
	}

	void GamePlayNetSystem::ProcessOutgoingMessages(uint32_t tick)
	{
		PlayerId myId = network_->GetMyPlayerID();

		auto currInput = currentStatePtr_->GetPlayerInputTimelines()[myId][tick];

		InputCommand inputCmd{
			myId,
			currInput
		};
		//std::cout << inputCmd.snapshot.tick << " " << (int)inputCmd.snapshot.down << " " << (int)inputCmd.snapshot.held << '\n';
		if(currInput.tick == tick)
			network_->SendUDPInputPacket(inputCmd);
	}
}

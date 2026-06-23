#include "GameNetwork/GamePlayNetSystem.h"
#include "Network/ClientNetwork.h"

#include "GameNetwork/Message/NetMessage.h"
#include "GameNetwork/Message/InputMessage.h"

#include "State/State.h"
#include "ECS/SystemUpdateContexts.h"

namespace tomato
{
	GamePlayNetSystem::GamePlayNetSystem(State* state)
		: currentStatePtr_(state)
	{
	}

	void GamePlayNetSystem::HandleInput(const InputCommand& inputCmd)
	{
		currentStatePtr_->SetPlayerInput(inputCmd.inputRecord.tick, inputCmd.inputRecord, inputCmd.id);
        latestTick_ = std::min(latestTick_, inputCmd.inputRecord.tick);

		//std::cout << inputCmd.inputRecord.tick << " " << (int)inputCmd.inputRecord.down << " " << (int)inputCmd.inputRecord.held << '\n';
		//currentStatePtr_->GetPlayerInputTimelines()[inputCmd.id][inputCmd.inputRecord.tick] = inputCmd.inputRecord;
		//std::cout << "[ INPUT ] " << inputCmd.inputRecord.tick << " " << (int)inputCmd.inputRecord.down << " " << (int)inputCmd.inputRecord.held << '\n';
	}

	void GamePlayNetSystem::ProcessOutgoingMessages(uint32_t tick)
	{
		PlayerId myId = network_->GetMyPlayerID();

		InputCommand inputCmd{
			myId,
			tick,
			currentStatePtr_->GetPlayerInputTimelines()[myId][tick]
		};
		//std::cout << inputCmd.inputRecord.tick << " " << (int)inputCmd.inputRecord.down << " " << (int)inputCmd.inputRecord.held << '\n';
		network_->SendUDPInputPacket(inputCmd);
	}
}

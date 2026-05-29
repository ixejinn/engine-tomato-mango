#include "GameNetwork/GamePlayNetSystem.h"
#include "Network/ClientNetwork.h"

#include "GameNetwork/Message/NetMessage.h"
#include "GameNetwork/Message/InputMessage.h"

#include "State/State.h"
#include "ECS/SystemUpdateContexts.h"

tomato::GamePlayNetSystem::GamePlayNetSystem(State* state, ClientNetwork* network, SPSCQueue<InputCommand, 256>& inputQueue, SPSCQueue<NetMessage*, 256>& dataQueue)
	: currentStatePtr_(state), network_(network), inputCmdQueue(inputQueue), gameDataQueue(dataQueue)
{
}

void tomato::GamePlayNetSystem::BeginFrame()
{
	auto& r = currentStatePtr_->GetRegistry();
	while (!inputCmdQueue.Empty())
	{
		InputCommand inputCmd;
		inputCmdQueue.Dequeue(inputCmd);

		auto inputCtx = r.ctx().get<InputContext*>();
		inputCtx->timelines[inputCmd.id][inputCmd.tick] = inputCmd.inputRecord;
	}

	/* @TODO : add net message registry
	while (!gameDataQueue.Empty())
	{
		NetMessage* gameData;
		gameDataQueue.Dequeue(gameData);

		//gameData->Read();
	}
	*/
}

void tomato::GamePlayNetSystem::EndFrame()
{
	auto& r = currentStatePtr_->GetRegistry();
	//auto inputCtx = r.ctx().get<InputContext*>();
	//network_->SendUDPPacket(UDPPacketType::INPUT, SendPolicy::Broadcast);
}

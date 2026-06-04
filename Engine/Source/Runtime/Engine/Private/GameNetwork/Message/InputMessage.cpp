#include <limits>

#include "GameNetwork/Message/InputMessage.h"

#include "Network/NetBitReader.h"
#include "Network/NetBitWriter.h"
#include "Network/SocketAddress.h"
#include "Network/ClientNetwork.h"

#include "ECS/SystemUpdateContexts.h"

namespace tomato
{
    //void InputNetMessage::Build(SimContext& ctx, ClientNetwork* network)
    //{
    //    tick = ctx.tick;

    //    auto input = ctx.registry.ctx().get<InputContext*>();
    //    //auto net = ctx.registry.ctx().get<NetworkContext*>();

    //    inputRecord = input->timelines[network->GetMyPlayerID()][tick];
    //    //inputRecord = engine.GetInputTimeline()[engine.GetNetworkService().GetMyPlayerID()][tick];
    //}

    //void InputNetMessage::Apply(const SocketAddress& fromAddr, SimContext& ctx, ClientNetwork* network)
    //{
    //    //engine.SetInputData(engine.GetNetworkService().GetPeerPlayerID(fromAddr), inputRecord);
    //    //engine.SetLatestTick(inputRecord.tick);

    //    auto input = ctx.registry.ctx().get<InputContext*>();
    //    input->timelines[network->GetPeerPlayerID(fromAddr)][inputRecord.tick] = inputRecord;
    //    //ctx.tick = std::min(tick, inputRecord.tick);
    //}

    void InputCommand::Read(NetBitReader& reader)
    {
        reader.ReadInt(inputRecord.tick, std::numeric_limits<uint32_t>::max());

        uint16_t value = 0;
        reader.ReadInt(value, uint16_t(InputIntent::COUNT));
        inputRecord.down = static_cast<InputIntent>(value);
        reader.ReadInt(value, uint16_t(InputIntent::COUNT));
        inputRecord.held = static_cast<InputIntent>(value);
        std::cout << "[ INPUT ] " << inputRecord.tick << " " << (int)inputRecord.down << " " << (int)inputRecord.held << '\n';
    }

    void InputCommand::Write(NetBitWriter& writer)
    {
        writer.WriteInt(tick, std::numeric_limits<uint32_t>::max());
        writer.WriteInt(static_cast<uint16_t>(inputRecord.down), static_cast<uint16_t>(InputIntent::COUNT));
        writer.WriteInt(static_cast<uint16_t>(inputRecord.held), static_cast<uint16_t>(InputIntent::COUNT));
        //std::cout << "Write : " << tick << '\n';
        std::cout << "[ OUT ] " << inputRecord.tick << " " << (int)inputRecord.down << " " << (int)inputRecord.held << '\n';
    }
}
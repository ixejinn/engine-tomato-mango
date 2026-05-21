#include <limits>

#include "GameNetwork/Message/InputMessage.h"

#include "Network/NetBitReader.h"
#include "Network/NetBitWriter.h"
#include "Network/SocketAddress.h"
#include "Network/NetworkService.h"

#include "ECS/SystemUpdateContexts.h"

namespace tomato
{
    void InputNetMessage::Serialize(NetBitWriter& writer)
    {
        writer.WriteInt(tick, std::numeric_limits<uint32_t>::max());
        writer.WriteInt(static_cast<uint16_t>(inputRecord.down), static_cast<uint16_t>(InputIntent::COUNT));
        writer.WriteInt(static_cast<uint16_t>(inputRecord.held), static_cast<uint16_t>(InputIntent::COUNT));
    }

    void InputNetMessage::Build(SimContext& ctx, NetworkService* network)
    {
        tick = ctx.tick;

        auto input = ctx.registry.ctx().get<InputContext*>();
        //auto net = ctx.registry.ctx().get<NetworkContext*>();

        inputRecord = input->timelines[network->GetMyPlayerID()][tick];
        //inputRecord = engine.GetInputTimeline()[engine.GetNetworkService().GetMyPlayerID()][tick];
    }

    void InputNetMessage::Deserialize(NetBitReader& reader)
    {
        reader.ReadInt(inputRecord.tick, std::numeric_limits<uint32_t>::max());

        uint16_t value = 0;
        reader.ReadInt(value, uint16_t(InputIntent::COUNT));
        inputRecord.down = static_cast<InputIntent>(value);
        reader.ReadInt(value, uint16_t(InputIntent::COUNT));
        inputRecord.held = static_cast<InputIntent>(value);
    }

    void InputNetMessage::Apply(const SocketAddress& fromAddr, SimContext& ctx, NetworkService* network)
    {
        //engine.SetInputData(engine.GetNetworkService().GetPeerPlayerID(fromAddr), inputRecord);
        //engine.SetLatestTick(inputRecord.tick);

        auto input = ctx.registry.ctx().get<InputContext*>();
        input->timelines[network->GetPeerPlayerID(fromAddr)][tick] = inputRecord;
        ctx.tick = std::min(tick, inputRecord.tick);
    }
}
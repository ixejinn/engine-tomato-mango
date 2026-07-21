#include <limits>

#include "GameNetwork/Message/InputMessage.h"

#include "Network/NetBitReader.h"
#include "Network/NetBitWriter.h"
#include "Network/SocketAddress.h"
#include "Network/ClientNetwork.h"

#include "ECS/SystemFramework/SystemUpdateContexts.h"

namespace tomato
{
    void InputCommand::Read(NetBitReader& reader)
    {
        reader.ReadInt(inputRecord.tick, std::numeric_limits<uint32_t>::max());

        uint32_t value = 0;
        reader.ReadInt(value, uint32_t(InputIntent::COUNT));
        inputRecord.down = static_cast<InputIntent>(value);
        reader.ReadInt(value, uint32_t(InputIntent::COUNT));
        inputRecord.held = static_cast<InputIntent>(value);

        //std::cout << "[ READ ] " << inputRecord.tick << " " << (int)inputRecord.down << " " << (int)inputRecord.held << '\n';
    }

    void InputCommand::Write(NetBitWriter& writer)
    {
        writer.WriteInt(inputRecord.tick, std::numeric_limits<uint32_t>::max());
        writer.WriteInt(static_cast<uint32_t>(inputRecord.down), static_cast<uint32_t>(InputIntent::COUNT));
        writer.WriteInt(static_cast<uint32_t>(inputRecord.held), static_cast<uint32_t>(InputIntent::COUNT));
        
        //std::cout << "Write : " << tick << '\n';
        //std::cout << "[ WRITE ] " << inputRecord.tick << " " << (int)inputRecord.down << " " << (int)inputRecord.held << '\n';
    }
}
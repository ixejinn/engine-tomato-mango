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
        reader.ReadInt(snapshot.tick, std::numeric_limits<uint32_t>::max());

        uint32_t value = 0;
        reader.ReadInt(value, uint32_t(InputAction::COUNT));
        snapshot.pressed = static_cast<InputAction>(value);
        reader.ReadInt(value, uint32_t(InputAction::COUNT));
        snapshot.held = static_cast<InputAction>(value);

        //std::cout << "[ READ ] " << snapshot.tick << " " << (int)snapshot.down << " " << (int)snapshot.held << '\n';
    }

    void InputCommand::Write(NetBitWriter& writer)
    {
        writer.WriteInt(snapshot.tick, std::numeric_limits<uint32_t>::max());
        writer.WriteInt(static_cast<uint32_t>(snapshot.pressed), static_cast<uint32_t>(InputAction::COUNT));
        writer.WriteInt(static_cast<uint32_t>(snapshot.held), static_cast<uint32_t>(InputAction::COUNT));
        
        //std::cout << "Write : " << tick << '\n';
        //std::cout << "[ WRITE ] " << snapshot.tick << " " << (int)snapshot.down << " " << (int)snapshot.held << '\n';
    }
}
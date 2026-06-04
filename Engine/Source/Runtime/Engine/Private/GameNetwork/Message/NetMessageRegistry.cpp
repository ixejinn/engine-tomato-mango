#include "GameNetwork/Message/NetMessageRegistry.h"

namespace tomato
{
    void NetMessageRegistry::RegisterFactory(const UDPPacketType type, NetMessageFactory&& factory)
    {
        factories_[type] = std::move(factory);
    }

    const NetMessageFactory& NetMessageRegistry::GetFactory(const UDPPacketType type)
    {
        return factories_[type];
    }
}
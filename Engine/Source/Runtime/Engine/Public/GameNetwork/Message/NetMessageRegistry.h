#ifndef MANGO_NETMESSAGEREGISTRY_H
#define MANGO_NETMESSAGEREGISTRY_H

#include <cstdint>
#include <functional>
#include <memory>
#include "Containers/EnumArray.h"
#include "Network/PacketTypes.h"

namespace tomato
{
    struct NetMessage;

    using NetMessageFactory = std::function<std::unique_ptr<NetMessage>()>;

    class NetMessageRegistry
    {
    private:
        NetMessageRegistry() = default;

    public:
        ~NetMessageRegistry() = default;

        NetMessageRegistry(const NetMessageRegistry&) = delete;
        NetMessageRegistry& operator=(const NetMessageRegistry&) = delete;

        void RegisterFactory(UDPPacketType type, NetMessageFactory&& factory);
        const NetMessageFactory& GetFactory(UDPPacketType type);

        static NetMessageRegistry& GetInstance()
        {
            static NetMessageRegistry instance;
            return instance;
        }

    private:
        EnumArray<UDPPacketType, NetMessageFactory> factories_{};
    };
}

#endif //MANGO_NETMESSAGEREGISTRY_H

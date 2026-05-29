#ifndef MANGO_NETMESSAGE_H
#define MANGO_NETMESSAGE_H

#include "Network/NetworkFwd.h"
#include "ECS/SystemFwd.h"

namespace tomato
{
    /**
     * @brief Base interface for network messages.
     *
     * @warning Serialize() and Deserialize() must remain strictly symmetric.
     */
    struct NetMessage
    {
    public:
        virtual ~NetMessage() = default;

        /**
         * @brief Deserializes the payload and applies it to the engine.
         * @param reader Bit reader containing the message payload.
         * @param engine Engine instance to apply the message.
         * @param fromAddr Source address of the received datagram.
         */
        void Read(NetBitReader& reader, SimContext& ctx, ClientNetwork* network, const SocketAddress& fromAddr)
        {
            Deserialize(reader);
            Apply(fromAddr, ctx, network);
        }

        /**
         * @brief Deserializes the payload and applies it without engine context.
         * @param reader Bit reader containing the message payload.
         * @param fromAddr Source address of the received datagram.
         */
        void Read(NetBitReader& reader, const SocketAddress& fromAddr)
        {
            Deserialize(reader);
            Apply(fromAddr);
        }

        /**
         * @brief Builds message data from the engine and serializes it.
         * @param writer Bit writer to append the serialized payload.
         * @param engine Engine instance used as a data source for serialization.
         *
         * Calls Build() followed by Serialize().
         */
        void Write(NetBitWriter& writer, SimContext& ctx, ClientNetwork* network)
        {
            Build(ctx, network);
            Serialize(writer);
        }

        /**
         * @brief Serializes the message without engine context.
         * @param writer Bit writer to append the serialized payload.
         */
        void Write(NetBitWriter& writer, ClientNetwork* network)
        {
            Build(network);
            Serialize(writer);
        }

    private:
        /**
         * @brief Serializes member variables into a bit-stream.
         * @warning Must remains symmetric with Deserialize().
         */
        virtual void Serialize(NetBitWriter&) = 0;

        virtual void Build(SimContext&, ClientNetwork*) {};
        virtual void Build(ClientNetwork* network) {};

        /**
         * @brief Deserializes member variables from a bit-stream.
         * @warning Must remains symmetric with Serialize().
         */
        virtual void Deserialize(NetBitReader&) = 0;

        virtual void Apply(const SocketAddress&, SimContext&, ClientNetwork* network) {};
        virtual void Apply(const SocketAddress&) {};
    };
}

#endif //MANGO_NETMESSAGE_H

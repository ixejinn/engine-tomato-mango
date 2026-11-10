#ifndef MANGO_NETWORKEVENTS_H
#define MANGO_NETWORKEVENTS_H

namespace tomato::Event
{
    struct NetworkModeRequest
    {
        bool client;
        bool server;
    };
}

#endif //MANGO_NETWORKEVENTS_H
#ifndef MANGO_STATEEVENT_H
#define MANGO_STATEEVENT_H

namespace tomato::Event
{
    struct ChangeStateRequest
    {
        const char* filepath;
    };
}

#endif //MANGO_STATEEVENT_H
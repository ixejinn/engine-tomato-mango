#ifndef MANGO_STATEEVENT_H
#define MANGO_STATEEVENT_H

#include <filesystem>

namespace tomato::Event
{
    struct ChangeStateRequest
    {
        //const char* filepath;
        std::filesystem::path filepath;
    };
}

#endif //MANGO_STATEEVENT_H
#ifndef MANGO_RENDERSORTENTRY_H
#define MANGO_RENDERSORTENTRY_H

#include <cstdint>
#include <entt/fwd.hpp>

namespace tomato
{
    struct RenderSortEntry
    {
        uint64_t key;
        entt::entity entity;
    };
}

#endif //MANGO_RENDERSORTENTRY_H
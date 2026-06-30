#ifndef MANGO_SYSTEMUPDATECONTEXTS_H
#define MANGO_SYSTEMUPDATECONTEXTS_H

#include <unordered_map>
#include <entt/entt.hpp>
#include "State/State.h"
#include "Network/ClientNetwork.h"
#include "Collision/CollisionFwd.h"

namespace tomato
{
    struct SimContext
    {
        SimContext(State* s, uint32_t t) : state(s), tick(t) {}

        State* state;
        uint32_t tick;
    };

    struct CollisionContext
    {
        std::unordered_map<CollisionPair, bool> collisionPairs;
    };

    struct RenderContext
    {
        entt::entity mainCam{entt::null};
    };

    struct UIContext
    {
        std::vector<entt::entity> drawList;
        std::vector<entt::entity> selectableList;

        bool selectableDirty{ true };
    };
}

#endif //MANGO_SYSTEMUPDATECONTEXTS_H
#ifndef MANGO_SYSTEMUPDATECONTEXTS_H
#define MANGO_SYSTEMUPDATECONTEXTS_H

#include <unordered_map>
#include <entt/entt.hpp>
#include <glm/vec3.hpp>
#include "State/State.h"
#include "Network/ClientNetwork.h"
#include "Collision/CollisionFwd.h"
#include "Collision/CollisionCache.h"

namespace tomato
{
    struct SimContext
    {
        SimContext(State* s, int64_t t) : state(s), tick(t) {}

        State* state;
        int64_t tick;
    };

    struct CollisionContext
    {
        std::unordered_map<ContactPair, ContactCache> cacheMap;
    };

    struct RenderContext
    {
        entt::entity mainCam{entt::null};
        entt::entity editorCam{entt::null};
        entt::entity playerCam{entt::null};

        entt::entity skybox{entt::null};
        entt::entity viewGizmo{entt::null};
    };

    struct UIContext
    {
        std::vector<entt::entity> screenDrawList;
        std::vector<entt::entity> worldDrawList;

        std::vector<entt::entity> selectableList;

        bool selectableDirty{ true };
    };
}

#endif //MANGO_SYSTEMUPDATECONTEXTS_H
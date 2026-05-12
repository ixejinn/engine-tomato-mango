#ifndef MANGO_SYSTEMUPDATECONTEXTS_H
#define MANGO_SYSTEMUPDATECONTEXTS_H

#include <entt/entt.hpp>
#include "State/StateFwd.h"

namespace tomato {
    struct SimContext {
        SimContext(entt::registry& reg, uint32_t t) : registry(reg), tick(t) {}

        entt::registry& registry;
        uint32_t tick;
    };

    struct InputContext {
        InputContext(std::array<Timeline<InputRecord>, MAX_PLAYER_NUM>& tl) : timelines(tl) {}

        std::array<Timeline<InputRecord>, MAX_PLAYER_NUM>& timelines;
    };

    struct RenderContext {
        RenderContext(int w, int h) : width(w), height(h), mainCam(entt::null) {}

        int width, height;
        entt::entity mainCam;
    };

    struct UIContext
    {
        std::vector<entt::entity> drawList;
        std::vector<entt::entity> selectableList;

        bool selectableDirty{ true };
    };
}

#endif //MANGO_SYSTEMUPDATECONTEXTS_H
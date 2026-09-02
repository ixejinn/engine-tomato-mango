#ifndef MANGO_CHARACTERSCALESYSTEM_H
#define MANGO_CHARACTERSCALESYSTEM_H

#include <entt/fwd.hpp>
#include "ECS/Systems/System.h"
#include "ECS/Forward/EntityCompFwd.h"

namespace tomato
{
    class CharacterScaleSystem : public System
    {
    public:
        void Update(SimContext& simCtx) override;

    private:
        static void UpdateFrom(entt::registry& reg, HierarchyComponent* hierarchy, float yScl);
    };
}

#endif //MANGO_CHARACTERSCALESYSTEM_H
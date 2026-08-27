#ifndef MANGO_PARTICLEEMITTERPOOL_H
#define MANGO_PARTICLEEMITTERPOOL_H

#include <entt/fwd.hpp>
#include <glm/vec3.hpp>

#include "Containers/EntityPool.h"
#include "Resource/ResourceFwd.h"
#include "UUID.h"

namespace tomato
{
    struct ParticlePoolTraits
    {
        static void Assemble(entt::registry& registry_, entt::entity e);
        static void Reset(entt::registry& registry_, entt::entity e,
            AssetID ptcID, glm::vec3 pos);
        static void Reset(entt::registry& registry_, entt::entity e,
            AssetID ptcID, UUID target, glm::vec3 offset = { 0.f, 0.f, 0.f });
        static bool Deactivate(entt::registry& registry_, entt::entity e);
    };

    using ParticleEmitterPool = EntityPool<ParticlePoolTraits>;
}

#endif //MANGO_PARTICLEEMITTERPOOL_H
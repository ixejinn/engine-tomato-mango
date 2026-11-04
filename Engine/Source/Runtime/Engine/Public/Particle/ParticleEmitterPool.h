#ifndef MANGO_PARTICLEEMITTERPOOL_H
#define MANGO_PARTICLEEMITTERPOOL_H

#include <entt/fwd.hpp>
#include <glm/vec3.hpp>

#include "Containers/EntityPool.h"
#include "Resource/ResourceFwd.h"

namespace tomato
{
    struct ParticleEmitterPoolTraits
    {
        static constexpr uint32_t N = 4;    // 동시에 재생 가능한 이미터 수
        static constexpr const char* Name = "Particle Effect";

        static void Assemble(entt::registry& registry, entt::entity e);

        /**
         * @param pos 파티클을 고정시킬 위치
         */
        static void Reset(entt::registry& registry, entt::entity e,
                          glm::vec3 pos, AssetID effectID = 0);
        /**
         * @param target 파티클이 따라다닐 엔티티
         * @param offset 엔티티 위치 기준
         */
        static void Reset(entt::registry& registry, entt::entity e,
                          entt::entity target, AssetID effectID = 0, glm::vec3 offset = {0, 0, 0});

        static bool Deactivate(entt::registry& registry, entt::entity e);
    };

    using ParticleEmitterPool = EntityPool<ParticleEmitterPoolTraits>;
}

#endif //MANGO_PARTICLEEMITTERPOOL_H
#ifndef MANGO_TRANSFORMSYSTEM_H
#define MANGO_TRANSFORMSYSTEM_H

#include <entt/fwd.hpp>
#include <glm/fwd.hpp>
#include "ECS/Systems/System.h"

namespace tomato {
    class TransformSystem : public System {
    public:
        void Update(SimContext &simCtx) override;

    private:
        void UpdateScreenUI(SimContext& simCtx);

    private:
        static void UpdateFrom(
            entt::registry& reg, entt::entity cur,
            const glm::quat& pQuat, const glm::vec3& pScale,
            const glm::mat4& pMatrix, bool pDirty);
    };
}

#endif //MANGO_TRANSFORMSYSTEM_H
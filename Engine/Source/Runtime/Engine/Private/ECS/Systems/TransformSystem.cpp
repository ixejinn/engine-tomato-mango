#include "ECS/Systems/TransformSystem.h"
#include "ECS/Components/Transform.h"
#include "ECS/SystemUpdateContexts.h"
#include "Utils/RegistryEntry.h"
REGISTER_SYSTEM(tomato::SystemPhase::Physics, TransformSystem)

namespace tomato {
    void TransformSystem::Update(SimContext &simCtx) {
        auto view = simCtx.registry.view<TransformComponent>();

        for (auto [e, trf] : view.each()) {
            // Scale → Rotate → Translate
            auto T = glm::translate(glm::mat4(1.f), trf.position);
            if (trf.rotDirty)
            {
                trf.rotation = glm::quat(glm::radians(trf.eulerDegree));
                trf.rotDirty = false;
            }
            auto R = glm::toMat4(trf.rotation);
            auto S = glm::scale(glm::mat4(1.f), trf.scale);

            trf.transformMatrix = T * R * S;
        }
    }
}
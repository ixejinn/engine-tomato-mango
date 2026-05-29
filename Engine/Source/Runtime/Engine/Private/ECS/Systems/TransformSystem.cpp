#include "ECS/Systems/TransformSystem.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Components/Camera.h"
#include "ECS/SystemUpdateContexts.h"
#include "Utils/RegistryEntry.h"
REGISTER_SYSTEM(tomato::SystemPhase::Transformation, TransformSystem)

namespace tomato {
    void TransformSystem::Update(SimContext &simCtx) {
        auto view = simCtx.registry.view<TransformComponent>();

        for (auto [e, trf] : view.each()) {
            if (!trf.dirty)
                continue;

            // Scale → Rotate → Translate
            auto T = glm::translate(glm::mat4(1.f), trf.position);
            auto R = glm::toMat4(trf.rotation);
            auto S = glm::scale(glm::mat4(1.f), trf.scale);

            trf.transformMatrix = T * R * S;

            trf.dirty = false;
            if (auto cam = simCtx.registry.try_get<CameraComponent>(e))
                cam->dirty = true;
        }
    }
}
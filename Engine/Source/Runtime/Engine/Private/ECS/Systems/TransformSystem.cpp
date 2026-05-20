#include "ECS/Systems/TransformSystem.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Components/Movement.h"
#include "ECS/SystemUpdateContexts.h"
#include "SimulationConfig.h"
#include "Utils/RegistryEntry.h"
REGISTER_SYSTEM(tomato::SystemPhase::Transformation, TransformSystem)

namespace tomato {
    void TransformSystem::Update(SimContext &simCtx) {
        auto view = simCtx.registry.view<TransformComponent>();

        for (auto [e, trf] : view.each()) {
            if (auto velocity = simCtx.registry.try_get<VelocityComponent>(e)) {
                // if (!trf.posDirty) {
                    trf.position += velocity->velocity * FIXED_DELTA_TIME;

                    // // !!!!! TEMP !!!!!
                    // if (trf.position.y < 0) {
                    //     trf.position.y = 0;
                    //     velocity->velocity.y = 0;
                    //
                    //     if (auto move = simCtx.registry.try_get<MovementComponent>(e))
                    //         move->jumpCnt = 0;
                    // }
                // }

                trf.posDirty = false;
            }

            // Scale → Rotate → Translate
            auto T = glm::translate(glm::mat4(1.f), trf.position);
            auto R = glm::toMat4(trf.rotation);
            auto S = glm::scale(glm::mat4(1.f), trf.scale);

            trf.transformMatrix = T * R * S;
        }
    }
}
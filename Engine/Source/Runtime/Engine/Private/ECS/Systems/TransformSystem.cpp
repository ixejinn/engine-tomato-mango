#include "ECS/Systems/TransformSystem.h"

#include "ECS/Components/Transform.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/UI.h"

#include "ECS/Components/Rigidbody.h"
#include "ECS/Components/Camera.h"
#include "ECS/SystemUpdateContexts.h"

#include "Utils/RegistryEntry.h"
REGISTER_SYSTEM(tomato::SystemPhase::Transformation, TransformSystem)

namespace tomato {
    void TransformSystem::Update(SimContext &simCtx) {
        auto rootView = simCtx.registry.view<RootEntityTag, TransformComponent>();

        static constexpr glm::quat rootQuat = glm::quat(1.f, 0.f, 0.f, 0.f);
        static constexpr glm::vec3 rootScale = glm::vec3{1.f};
        static constexpr glm::mat4 rootMat = glm::mat4{1.f};

        for (auto [e, trf] : rootView.each())
            UpdateFrom(simCtx.registry, e, rootQuat, rootScale, rootMat, false);
    }

    void TransformSystem::UpdateFrom(
        entt::registry& reg, entt::entity cur,
        const glm::quat& pQuat, const glm::vec3& pScale,
        const glm::mat4& pMatrix, bool pDirty)
    {
        auto& trf = reg.get<TransformComponent>(cur);

        bool bUpdated = trf.dirty || pDirty;
        if (bUpdated) {
            // Scale → Rotate → Translate
            auto T = glm::translate(glm::mat4(1.f), trf.position);
            auto R = glm::toMat4(trf.lRotation);
            auto S = glm::scale(glm::mat4(1.f), trf.scale);

            trf.wRotation = pQuat * trf.lRotation;
            trf.wScale = pScale * trf.scale;
            trf.transformMatrix = pMatrix * (T * R * S);
            trf.dirty = false;

            if (auto* cam = reg.try_get<CameraComponent>(cur))
                cam->dirty = true;
        }

        if (auto* hierarchy = reg.try_get<HierarchyComponent>(cur)) {
            for (const auto child : hierarchy->children)
                UpdateFrom(reg, child, trf.wRotation, trf.wScale, trf.transformMatrix, bUpdated);
        }
    }
}
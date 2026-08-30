#include "ECS/Systems/TransformSystem.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Particle.h"
#include "ECS/Components/Target.h"
#include "ECS/Components/TransformDirty.h"
#include "ECS/Entity/Hierarchy.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "Utils/Bitmask/BitmaskOperators.h"
#include "Event/EventDispatcher.h"
#include "GameObject/Character/MovementMode.h"
#include "Profiler/CPUProfiler.h"

namespace tomato
{
    void TransformSystem::Update(SimContext &simCtx)
    {
        EventDispatcher::GetInstance().Update<LandingEvent>();

        CPU_PROFILER_BLOCK_BEGIN(TransformSystem::Update);
        auto& registry = simCtx.state->GetRegistry();
        SetRootEntityDirtyBit(registry);

        static constexpr glm::quat ROOT_QUAT = glm::quat(1.f, 0.f, 0.f, 0.f);
        static constexpr glm::vec3 ROOT_SCL = glm::vec3{1.f};
        static constexpr glm::mat4 ROOT_MAT = glm::mat4{1.f};

        auto rootView = registry.view<RootEntityTag, TransformComponent>();
        for (auto [e, trf] : rootView.each())
        {
            if (HasFlag(trf.dirty, Transform::Dirty::Local | Transform::Dirty::Hierarchy))
                UpdateFrom(registry, e, ROOT_QUAT, ROOT_SCL, ROOT_MAT, false);
        }

        UpdateParticleTransform(registry);
        CPU_PROFILER_BLOCK_END(TransformSystem::Update);
    }

    void TransformSystem::SetRootEntityDirtyBit(entt::registry& reg)
    {
        auto view = reg.view<TransformComponent, HierarchyComponent>();
        for (auto [e, trf, hierarchy] : view.each())
        {
            if (!HasFlag(trf.dirty, Transform::Dirty::Local))
                continue;

            if (hierarchy.parent == entt::null)
            {
                trf.dirty |= Transform::Dirty::Hierarchy;
                continue;
            }

            reg.get<TransformComponent>(GetRootEntity(reg, e)).dirty |= Transform::Dirty::Hierarchy;
        }
    }

    void TransformSystem::UpdateFrom(
        entt::registry& reg, entt::entity cur,
        const glm::quat& pQuat, const glm::vec3& pScale,
        const glm::mat4& pMatrix, bool pDirty)
    {
        auto& trf = reg.get<TransformComponent>(cur);

        bool bUpdated = HasFlag(trf.dirty, Transform::Dirty::Local) || pDirty;
        if (bUpdated)
        {
            // Scale → Rotate → Translate
            auto T = glm::translate(glm::mat4(1.f), trf.position);
            auto R = glm::toMat4(trf.rotation);
            auto S = glm::scale(glm::mat4(1.f), trf.scale);

            trf.wRotation = pQuat * trf.rotation;
            trf.wScale = pScale * trf.scale;
            trf.transformMatrix = pMatrix * (T * R * S);
            trf.dirty = Transform::Dirty::None;

            if (auto* cam = reg.try_get<CameraComponent>(cur))
                cam->dirty = true;

            if (auto* col = reg.try_get<ColliderComponent>(cur))
                col->aabbDirty = true;
        }

        if (auto* hierarchy = reg.try_get<HierarchyComponent>(cur))
        {
            for (const auto child : hierarchy->children)
            {
                if (!reg.any_of<TransformComponent>(child))
                    continue;

                UpdateFrom(reg, child, trf.wRotation, trf.wScale, trf.transformMatrix, bUpdated);
            }
        }
    }
    void TransformSystem::UpdateParticleTransform(entt::registry& reg)
    {
        auto rootView = reg.view<TransformComponent, ParticleRuntimeComponent, TargetComponent>();

        for (auto [e, trf, runtime, target] : rootView.each())
        {
            if (target.target == 0) continue;
            if (!reg.valid(GetEntityByUUID(reg, target.target)))
                continue;

            auto& targetTrf = reg.get<TransformComponent>(GetEntityByUUID(reg, target.target));

            auto T = glm::translate(glm::mat4(1.f), trf.position);
            auto R = glm::toMat4(trf.rotation);
            auto S = glm::scale(glm::mat4(1.f), trf.scale);

            trf.wRotation = targetTrf.wRotation * trf.rotation;
            trf.wScale = targetTrf.wScale * trf.scale;
            trf.transformMatrix = targetTrf.transformMatrix * (T * R * S);
        }
    }
}
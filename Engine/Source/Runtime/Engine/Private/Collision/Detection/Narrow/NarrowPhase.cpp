#include <entt/entt.hpp>
#include "Collision/Detection/Narrow/NarrowPhase.h"
#include "Collision/Detection/Narrow/GJK/GJK.h"
#include "Collision/CollisionTolerance.h"
#include "Collision/Detection/Narrow/Pairs/SphereSphere.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Entity/Hierarchy.h"
#include "Simulation/SimulationConfig.h"

namespace tomato
{
    NarrowPhase::NarrowPhase()
    {
        RegisterOverlap(ColliderType::Sphere, ColliderType::Sphere, &SphereSphere::Overlap);
        RegisterSweep(ColliderType::Sphere, ColliderType::Sphere, &SphereSphere::Sweep);
    }

    void NarrowPhase::RegisterOverlap(ColliderType a, ColliderType b, OverlapFn fn)
    {
        Register(overlap_, a, b, fn);
    }

    void NarrowPhase::RegisterSweep(ColliderType a, ColliderType b, SweepFn fn)
    {
        Register(sweep_, a, b, fn);
    }

    std::optional<ContactData> NarrowPhase::EvaluateContactPair(entt::registry& reg, const ContactPair& pair)
    {
        const ShapeView a{reg.get<ColliderComponent>(pair.a), reg.get<TransformComponent>(pair.a)};
        const ShapeView b{reg.get<ColliderComponent>(pair.b), reg.get<TransformComponent>(pair.b)};

        if (a.col.trigger || b.col.trigger)
        {
            if (Overlap(a, b))
                return ContactData{};

            return std::nullopt;
        }

        glm::vec3 vA{0};
        glm::vec3 vB{0};
        if (auto* velA = reg.try_get<VelocityComponent>(GetRootEntity(reg, pair.a)))
            vA = velA->Get();
        if (auto* velB = reg.try_get<VelocityComponent>(GetRootEntity(reg, pair.b)))
            vB = velB->Get();
        glm::vec3 rv = vA - vB;

        const glm::vec3 ray = glm::length2(rv) < RELATIVE_TOLERANCE_SQ ? glm::vec3{0} : -rv * FIXED_DELTA_TIME;
        return Sweep(a, b, ray);
    }

    bool NarrowPhase::Overlap(const ShapeView& a, const ShapeView& b)
    {
        const auto& entry = overlap_[a.col.type][b.col.type];
        if (!entry.fn)
            return GJK::Overlap(a, b);
        return entry.reversed ? entry.fn(b, a) : entry.fn(a, b);
    }

    std::optional<ContactData> NarrowPhase::Sweep(const ShapeView& a, const ShapeView& b, const glm::vec3& ray)
    {
        const auto& entry = sweep_[a.col.type][b.col.type];
        if (!entry.fn)
            return GJK::Sweep(a, b, ray);
        return entry.reversed ? entry.fn(b, a, ray) : entry.fn(a, b, ray);
    }
}

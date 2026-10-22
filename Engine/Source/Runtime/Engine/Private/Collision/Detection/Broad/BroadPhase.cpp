#include <entt/entt.hpp>
#include "Collision/Detection/Broad/BroadPhase.h"
#include "Collision/Layer/CollisionLayerMatrix.h"
#include "Collision/CollisionConstants.h"
#include "ECS/Components/ActiveTag.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Components/PhysicsBody.h"
#include "ECS/Entity/Hierarchy.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "Simulation/SimulationConfig.h"

namespace tomato
{
    void BroadPhase::FindContactPairCandidates(entt::registry& reg, std::vector<ContactPair>& candidates)
    {
        filterEntries_.clear();
        proxies_.clear();
        overlaps_.clear();

        // Update AABBs and filter entries
        GatherProxies(reg);

        // Find overlapping AABB pairs
        sap_.FindOverlappingPairs(proxies_, overlaps_);

        // Filter
        auto& layerMtx = *reg.ctx().get<CollisionContext>().layerMtx;

        for (const auto& pair : overlaps_)
        {
            const FilterEntry& eA = filterEntries_[pair.idxA];
            const FilterEntry& eB = filterEntries_[pair.idxB];

            if (!CanCollide(layerMtx, eA, eB))
                continue;

            candidates.emplace_back(eA.entity, eB.entity);
        }
    }

    bool BroadPhase::CanCollide(const CollisionLayerMatrix& layerMtx, const FilterEntry& a, const FilterEntry& b)
    {
        if (a.trigger && b.trigger)
            return false;

        if (!layerMtx.CanCollide(a.layer, b.layer))
            return false;

        if (a.root == b.root)
            return false;

        if (!a.movable && !b.movable)
            return false;

        return true;
    }

    void BroadPhase::GatherProxies(entt::registry& reg)
    {
        auto view = reg.view<ActiveTag, ColliderComponent, TransformComponent>();
        for (auto [e, col, trf] : view.each())
        {
            const entt::entity root = GetRootEntity(reg, e);

            // Update AABB
            if (col.aabbDirty)
            {
                col.aabbDirty = false;

                // Sweep AABB center
                glm::vec3 wPos = trf.GetWorldPosition();
                if (const auto* vel = reg.try_get<VelocityComponent>(root))
                    wPos += vel->Get() * FIXED_DELTA_TIME;

                UpdateAABB(col.aabb, wPos, trf.GetWorldScale() * 0.50f,
                    glm::toMat4(trf.GetWorldQuaternion()), HALF_COLLISION_SKIN + 1e-6f);
            }

            // Update filter entries and proxies
            const auto* body = reg.try_get<PhysicsBodyComponent>(root);
            const bool movable = body && (body->inverseMass != 0);

            const auto filterEntryIdx = static_cast<uint32_t>(filterEntries_.size());

            filterEntries_.emplace_back(e, root, col.layer, col.trigger, movable);
            proxies_.emplace_back(col.aabb, entt::to_integral(e), filterEntryIdx);
        }
    }
}

#include <list>
#include <entt/entt.hpp>
#include "Collision/Broad/SAP.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Entity/Hierarchy.h"
#include "Simulation/SimulationConfig.h"
#include "Math/AABB.h"

namespace tomato
{
    void SAP::FindContactPairCandidates(entt::registry &reg, std::vector<ContactPair> &candidates)
    {
        UpdateAABBs(reg);

        auto group = reg.group<ColliderComponent>();

        // Sort by AABB.min.x for x-axis SAP
        group.sort<ColliderComponent>(
            [](const auto& a, const auto& b) { return a.aabb.min.x < b.aabb.min.x; },
            entt::insertion_sort{});

        // Active list contains AABBs that are currently open on the sweep axis.
        std::list<entt::entity> active;
        float activeMaxX = std::numeric_limits<float>::lowest();

        for (auto [e, col] : group.each())
        {
            AABB& aabb = col.aabb;

            if (activeMaxX < aabb.min.x)
            {
                // Initialize active list
                active.clear();

                active.push_back(e);
                activeMaxX = aabb.max.x;
            }
            else
            {
                // Check active list
                for (auto it = active.begin(); it != active.end();)
                {
                    auto& colAct = reg.get<ColliderComponent>(*it);
                    AABB& aabbAct = colAct.aabb;

                    // If active AABB.max < current AABB.min
                    // active AABB does not overlap on the sweep axis and cannot collide.
                    if (aabbAct.max.x < aabb.min.x)
                    {
                        active.erase(it++);
                        continue;
                    }

                    // Check collision layer
                    if (!CanCollide(reg, e, *it, col.layer, colAct.layer))
                    {
                        ++it;
                        continue;
                    }

                    // Test AABB (for axis y and z)
                    if (!TestAABBAABB(aabbAct, aabb))
                    {
                        ++it;
                        continue;
                    }

                    candidates.emplace_back(e, *it);
                    ++it;
                }

                active.push_back(e);
                activeMaxX = std::max(activeMaxX, aabb.max.x);
            }
        }
    }

    void SAP::UpdateAABBs(entt::registry& reg)
    {
        // Update AABB
        auto view = reg.view<ColliderComponent, TransformComponent>();
        for (auto [e, col, trf] : view.each())
        {
            if (!col.aabbDirty)
                continue;
            col.aabbDirty = false;

            // Sweep AABB center
            glm::vec3 wPos = trf.GetWorldPosition();
            if (const auto velPtr = reg.try_get<VelocityComponent>(GetRootEntity(reg, e)))
                wPos += velPtr->velocity * FIXED_DELTA_TIME;

            UpdateAABB(col.aabb, wPos, trf.GetWorldScale() * 0.5f,
                glm::toMat4(trf.GetWorldQuaternion()), HALF_COLLISION_SKIN + 1e-6f);
        }
    }
}

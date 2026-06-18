#include <list>
#include <entt/entt.hpp>
#include "Collision/Broad/SAP.h"
#include "Collision/CollisionPair.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Hierarchy.h"

namespace tomato {
    void SAP::FindCollisionCandidates(entt::registry &reg, std::vector<CollisionPair> &candidates) {
        auto group = reg.group<ColliderComponent>();

        // Sort by AABB.min.x for x-axis SAP
        group.sort<ColliderComponent>([](const auto& l, const auto& r)
                                      { return l.min.x < r.min.x; });

        // Active list contains AABBs that are currently open on the sweep axis.
        std::list<entt::entity> active;
        float activeMaxX = std::numeric_limits<float>::lowest();

        for (auto [e, col] : group.each())
        {
            // Initialize active list
            if (activeMaxX < col.min.x)
            {
                active.clear();

                active.push_back(e);
                activeMaxX = col.max.x;
            }
            else
            {
                for (auto it = active.begin(); it != active.end();)
                {
                    auto& colAct = reg.get<ColliderComponent>(*it);

                    // If active AABB.max < current AABB.min
                    // active AABB does not overlap on the sweep axis and cannot collide.
                    if (colAct.max.x < col.min.x)
                    {
                        active.erase(it++);
                        continue;
                    }

                    if (!CanCollide(reg, e, *it, col, colAct)) {
                        ++it;
                        continue;
                    }

                    // Check AABB
                    if (!CheckAABBAxisY(col, colAct) || !CheckAABBAxisZ(col, colAct)) {
                        ++it;
                        continue;
                    }

                    candidates.emplace_back(e, *it, (col.isTrigger || colAct.isTrigger));
                    ++it;
                }

                active.push_back(e);
                activeMaxX = std::max(activeMaxX, col.max.x);
            }
        }
    }
}

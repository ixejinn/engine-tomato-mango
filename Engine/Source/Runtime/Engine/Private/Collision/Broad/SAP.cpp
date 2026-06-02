#include <list>
#include <entt/entt.hpp>
#include "Collision/Broad/SAP.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Systems/CollisionSystem.h"

namespace tomato {
    void SAP::DetectCollision(entt::registry &reg, std::vector<CollisionPair> &candidates) {
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

                    // // Check parent
                    // if (GetRootEntity(reg, e) == GetRootEntity(reg, *it))
                    //     continue;
                    //
                    // // Check collision layer
                    // if (!layerMatrix_.CanCollide(col.layer, colAct.layer))
                    // {
                    //     ++it;
                    //     continue;
                    // }

                    // Check AABB
                    if (!CheckAABBAxisY(col, colAct) || !CheckAABBAxisZ(col, colAct)) {
                        ++it;
                        continue;
                    }

                    // if (colAct.max.y < col.min.y || col.max.y < colAct.min.y)
                    // {
                    //     ++it;
                    //     continue;
                    // }
                    // if (colAct.max.z < col.min.z || col.max.z < colAct.min.z)
                    // {
                    //     ++it;
                    //     continue;
                    // }

                    candidates.emplace_back(e, *it);
                    ++it;
                }

                active.push_back(e);
                activeMaxX = std::max(activeMaxX, col.max.x);
            }
        }
    }
}

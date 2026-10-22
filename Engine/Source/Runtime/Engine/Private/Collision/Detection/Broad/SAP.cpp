#include <list>
#include <entt/entt.hpp>
#include "Collision/Detection/Broad/SAP.h"
#include "Collision/CollisionConstants.h"
#include "ECS/Components/ActiveTag.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Entity/Hierarchy.h"
#include "Simulation/SimulationConfig.h"
#include "Math/AABB.h"

namespace tomato
{
    void SAP::FindOverlappingPairs(std::vector<BroadPhaseProxy>& proxies, std::vector<OverlapEntryPair>& out)
    {
        // Sort proxies by AABB.min.x for x-axis SAP and OverlapEntryPair::sortKey
        std::ranges::sort(proxies, [](const BroadPhaseProxy& a, const BroadPhaseProxy& b)
        {
            return std::tie(a.aabb.min.x, a.sortKey) < std::tie(b.aabb.min.x, b.sortKey);
            // if (a.aabb.min.x != b.aabb.min.x)
            //     return a.aabb.min.x < b.aabb.min.x;
            // return a.sortKey < b.sortKey;
        });

        // Active list contains AABBs that are currently open on the sweep axis.
        active_.clear();    // for proxies index
        float activeMaxX = std::numeric_limits<float>::lowest();

        for (uint32_t i = 0; i < proxies.size(); ++i)
        {
            AABB& aabb = proxies[i].aabb;

            if (activeMaxX < aabb.min.x)
            {
                // Clear the active_ if no overlapping AABBs remain
                active_.clear();
                active_.push_back(i);
                activeMaxX = aabb.max.x;
                continue;
            }

            // Check active_
            size_t write = 0;
            for (size_t read = 0; read < active_.size(); ++read)
            {
                const uint32_t act = active_[read];
                AABB& aabbAct = proxies[act].aabb;

                // If active AABB.max < current AABB.min
                // active AABB does not overlap on the sweep axis and cannot collide
                if (aabbAct.max.x < aabb.min.x)
                    continue;

                active_[write++] = act; // read 위치의 원소(현재 AABB와 겹침)를 write 위치에 덮어쓰며 겹치지 않는 원소 제거 (압축)

                // Test AABB (for axis y and z)
                if (TestAABBAABB(aabbAct, aabb))
                    out.emplace_back(proxies[act].entryIndex, proxies[i].entryIndex);
            }
            active_.resize(write);
            active_.emplace_back(i);

            activeMaxX = std::max(activeMaxX, aabb.max.x);
        }
    }
}

#ifndef MANGO_BROADPHASE_H
#define MANGO_BROADPHASE_H

#include "Collision/CollisionFwd.h"
#include "Collision/Detection/Broad/SAP.h"
#include "ECS/Forward/PhysCompFwd.h"

namespace tomato
{
    class BroadPhase
    {
    public:
        void FindContactPairCandidates(entt::registry& reg, std::vector<ContactPair>& candidates);

    private:
        struct FilterEntry
        {
            entt::entity entity;
            entt::entity root;
            CollisionLayer layer;
            bool trigger;
            bool movable;
        };

        static bool CanCollide(const CollisionLayerMatrix& layerMtx, const FilterEntry& a, const FilterEntry& b);

        void GatherProxies(entt::registry& reg);

        SAP sap_;

        std::vector<FilterEntry> filterEntries_;    // 콜라이더별 필터에 필요한 정보, BroadPhaseProxy::entryIndex로 접근
        std::vector<BroadPhaseProxy> proxies_;      // SAP 정렬 및 overlaps_ 갱신에 사용되는 프록시
        std::vector<OverlapEntryPair> overlaps_;    // AABB가 겹치는 filterEntries_의 인덱스 쌍
    };
}

#endif //MANGO_BROADPHASE_H
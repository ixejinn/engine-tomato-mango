#ifndef MANGO_COLLISIONSYSTEM_H
#define MANGO_COLLISIONSYSTEM_H

#include <entt/fwd.hpp>
#include <vector>
#include <unordered_map>
#include <memory>
#include "ECS/Systems/System.h"
#include "Collision/CollisionEventFwd.h"
#include "Collision/CollisionFwd.h"
#include "Collision/CollisionPair.h"
#include "Collision/Narrow/GJK.h"

namespace tomato {
    class CollisionSystem : public System {
    public:
        CollisionSystem();
        ~CollisionSystem() override;

        void Update(SimContext& simCtx) override;

    private:
        void DetectBroad(SimContext& simCtx);
        void DetectNarrow(SimContext& simCtx);

        static void SetAABB(entt::registry& reg, entt::entity e);

        static void ResolveCollision(entt::registry& reg, entt::entity e1, entt::entity e2, const CollisionResult& info);

        static void OnPenetration(const PenetrationEvent& e);

        static void OnCollisionEnter(const CollisionEnterEvent& e);
        static void OnCollisionStay(const CollisionStayEvent& e);
        static void OnCollisionExit(const CollisionExitEvent& e);

        static void OnTriggerEnter(const TriggerEnterEvent& e);
        static void OnTriggerStay(const TriggerStayEvent& e);
        static void OnTriggerExit(const TriggerExitEvent& e);

        std::unique_ptr<BroadPhase> broadPhase_;
        std::unique_ptr<NarrowPhase> narrowPhase_;

        /// Broad-phase collision candidates for narrow-phase check.
        std::vector<CollisionPair> candidates_;

        /// Active collision pairs to determine ENTER/STAY/EXIT states.
        std::unordered_map<CollisionPair, bool> activePairs_;
    };
}

#endif //MANGO_COLLISIONSYSTEM_H
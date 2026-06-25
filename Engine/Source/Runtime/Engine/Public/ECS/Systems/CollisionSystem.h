#ifndef MANGO_COLLISIONSYSTEM_H
#define MANGO_COLLISIONSYSTEM_H

#include <entt/fwd.hpp>
#include <vector>
#include <memory>
#include "ECS/Systems/System.h"
#include "Collision/CollisionEventFwd.h"
#include "Collision/CollisionFwd.h"
#include "Collision/Narrow/GJK.h"

namespace tomato {
    class CollisionSystem : public System {
    public:
        CollisionSystem();
        ~CollisionSystem() override;

        void Update(SimContext& simCtx) override;

    private:
        void DetectBroad(entt::registry& reg);
        void DetectNarrow(SimContext& simCtx);

        static void SetAABB(entt::registry& reg, entt::entity e);

        static void SolveCollision(entt::registry& reg, entt::entity e1, entt::entity e2, const CollisionInfo& info);

        static void OnPenetration(const PenetrationEvent& e);

        static void OnCollisionEnter(const CollisionEnterEvent& e);
        static void OnCollisionStay(const CollisionStayEvent& e);
        static void OnCollisionExit(const CollisionExitEvent& e);

        static void OnTriggerEnter(const TriggerEnterEvent& e);
        static void OnTriggerStay(const TriggerStayEvent& e);
        static void OnTriggerExit(const TriggerExitEvent& e);

        std::unique_ptr<BroadPhase> broadPhase_;
        std::unique_ptr<NarrowPhase> narrowPhase_;

        std::vector<CollisionPair> candidates_;
    };
}

#endif //MANGO_COLLISIONSYSTEM_H
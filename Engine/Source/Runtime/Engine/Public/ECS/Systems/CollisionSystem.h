#ifndef MANGO_COLLISIONSYSTEM_H
#define MANGO_COLLISIONSYSTEM_H

#include <entt/fwd.hpp>
#include <vector>
#include <memory>
#include "ECS/Systems/System.h"
#include "Collision/CollisionEventFwd.h"
#include "Collision/CollisionFwd.h"
#include "Collision/Narrow/GJK/GJK.h"

namespace tomato
{
    class CollisionSystem : public System
    {
    public:
        CollisionSystem();
        ~CollisionSystem() override;

        void Update(SimContext& simCtx) override;

    private:
        static void UpdateAABB(entt::registry& reg);

        static void SolveCollision(entt::registry& reg, entt::entity e1, entt::entity e2, const CollisionInfo& info);

        static void OnPenetration(const PenetrationEvent& e);

        void RunBroadPhase(SimContext& simCtx);
        void RunNarrowPhase(SimContext& simCtx);

        std::unique_ptr<BroadPhase> broadPhase_;
        std::unique_ptr<NarrowPhase> narrowPhase_;

        std::vector<CollisionPair> candidates_;

        void ResolveCollision(entt::registry& reg);

        std::vector<CollisionEvent> events_;
    };
}

#endif //MANGO_COLLISIONSYSTEM_H
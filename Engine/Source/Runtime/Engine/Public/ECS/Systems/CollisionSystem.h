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
        static constexpr int EXIT_CNT = 2;
        static constexpr float CORRECTION_SPEED = 5.f;

        void RunBroadPhase(SimContext& simCtx);
        void RunNarrowPhase(SimContext& simCtx);
        std::unique_ptr<BroadPhase> broadPhase_;
        std::unique_ptr<NarrowPhase> narrowPhase_;

        std::vector<ContactPair> candidates_;

        static void UpdateAABB(entt::registry& reg);

        void ResolveContacts(entt::registry& reg);
        std::vector<CollisionEvent> contacts_;
        static void ResolveContact(entt::registry& reg, entt::entity e1, entt::entity e2, const ContactData& data);

        static void CorrectPenetration(const PenetrationEvent& e);
    };
}

#endif //MANGO_COLLISIONSYSTEM_H
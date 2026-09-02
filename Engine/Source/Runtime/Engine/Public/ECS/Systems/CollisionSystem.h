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

        void ResolveContacts();
        std::vector<ContactEvent> contacts_;
        static void ResolveContact(ContactEvent& event);

        static void ResolveContinuousContact(
            TransformComponent& trf, VelocityComponent& vel,
            const glm::vec3& normal, float weight, float hitTime);

        static void ResolveDiscreteContact(
            TransformComponent& trf, VelocityComponent& vel,
            const glm::vec3& normal, float weight, float distance);

        static void ResolvePenetration(
            TransformComponent& trf,
            const glm::vec3& normal, float weight, float distance);

        static void UpdateCollisionEvents();
    };
}

#endif //MANGO_COLLISIONSYSTEM_H
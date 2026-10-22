#ifndef MANGO_COLLISIONSYSTEM_H
#define MANGO_COLLISIONSYSTEM_H

#include <entt/fwd.hpp>
#include <vector>
#include <memory>
#include "ECS/Systems/System.h"
#include "Collision/CollisionEventFwd.h"
#include "Collision/CollisionFwd.h"
#include "Collision/Detection/Broad/BroadPhase.h"
#include "Collision/Detection/Narrow/NarrowPhase.h"

namespace tomato
{
    class CollisionSystem : public System
    {
    public:
        ~CollisionSystem() override;

        void Update(SimContext& simCtx) override;

    private:
        static constexpr int EXIT_CNT = 2;

        void RunBroadPhase(SimContext& simCtx);
        void RunNarrowPhase(SimContext& simCtx);
        BroadPhase broad_;
        NarrowPhase narrow_;

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

        static void UpdateCollisionEvents();
    };
}

#endif //MANGO_COLLISIONSYSTEM_H
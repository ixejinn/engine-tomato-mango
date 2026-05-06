#ifndef MANGO_COLLISIONSYSTEM_H
#define MANGO_COLLISIONSYSTEM_H

#include <entt/fwd.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>
#include "ECS/Systems/System.h"
#include "ECS/PhysCompFwd.h"
#include "Collision/CollisionEventFwd.h"
#include "Collision/CollisionFwd.h"
#include "Collision/Narrow/GJK.h"

namespace tomato {
    class CollisionSystem : public System {
    public:
        CollisionSystem();
        ~CollisionSystem();

        void Update(SimContext& simCtx) override;

    private:
        void BroadCheck(entt::registry& reg);
        void NarrowCheck(entt::registry& reg);

        static void SetAABB(ColliderComponent& col, const TransformComponent& trf);

        static void OnCollisionEnter(const CollisionEnterEvent& e);
        static void OnCollisionStay(const CollisionStayEvent& e);
        static void OnCollisionExit(const CollisionExitEvent& e);

        static void OnTriggerEnter(const TriggerEnterEvent& e);
        static void OnTriggerStay(const TriggerStayEvent& e);
        static void OnTriggerExit(const TriggerExitEvent& e);

        std::unique_ptr<BroadPhase> broadPhase_;
        std::unique_ptr<NarrowPhase> narrowPhase_;

        std::vector<CollisionPair> candidates_;
        std::unordered_map<CollisionPair, bool> collisionPairs_;
    };
}

#endif //MANGO_COLLISIONSYSTEM_H
#ifndef MANGO_COLLISIONSYSTEM_H
#define MANGO_COLLISIONSYSTEM_H

#include <entt/fwd.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <functional>
#include <unordered_map>
#include "ECS/Systems/System.h"
#include "ECS/PhysCompFwd.h"
#include "Collision/CollisionEventFwd.h"
#include "Collision/CollisionLayerMatrix.h"
#include "Collision/CollisionPair.h"
#include "Containers/EnumArray.h"

namespace tomato {
    class CollisionSystem : public System {
    public:
        CollisionSystem();

        void Update(SimContext& simCtx) override;

    private:
        void BroadPhase(entt::registry& reg);
        void NarrowPhase(entt::registry& reg);

        static void SetAABB(ColliderComponent& col, const TransformComponent& trf);
        void SAP(entt::registry& reg);

        bool GJK(
            const ColliderComponent& col1, const TransformComponent& trf1,
            const ColliderComponent& col2, const TransformComponent& trf2);
        glm::vec3 Support(
            const glm::vec3& worldDir,
            const ColliderComponent& col, const TransformComponent& trf);
        bool AddSimplexPoint(
            std::vector<glm::vec3>& simplex,
            const ColliderComponent& col1, const TransformComponent& trf1,
            const ColliderComponent& col2, const TransformComponent& trf2);

        static bool VoronoiRegion(std::vector<glm::vec3>& simplex);
        static glm::vec3 GetOrientedNormal(
            const glm::vec3& refP,
            const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2);

        static void OnCollisionEnter(const CollisionEnterEvent& e);
        static void OnCollisionStay(const CollisionStayEvent& e);
        static void OnCollisionExit(const CollisionExitEvent& e);

        static void OnTriggerEnter(const TriggerEnterEvent& e);
        static void OnTriggerStay(const TriggerStayEvent& e);
        static void OnTriggerExit(const TriggerExitEvent& e);

        CollisionLayerMatrix layerMatrix_;

        std::vector<CollisionPair> candidates_;

        using SupportFunc = std::function<glm::vec3(const glm::vec3& dir, const ColliderComponent& col)>;
        EnumArray<ColliderType, SupportFunc> supportFunctions_;

        std::unordered_map<CollisionPair, bool> collisionPairs_;
    };
}

#endif //MANGO_COLLISIONSYSTEM_H
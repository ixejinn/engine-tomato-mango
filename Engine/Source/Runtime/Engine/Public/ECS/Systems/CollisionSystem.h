#ifndef MANGO_COLLISIONSYSTEM_H
#define MANGO_COLLISIONSYSTEM_H

#include <entt/fwd.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include "ECS/Systems/System.h"
#include "ECS/PhysCompFwd.h"
#include "Collision/CollisionEventFwd.h"
#include "Collision/CollisionLayerMatrix.h"
#include "Containers/EnumArray.h"

namespace tomato {
    class CollisionSystem : public System {
    public:
        CollisionSystem();

        void Update(SimContext& simCtx) override;

    private:
        void BroadPhase(entt::registry& reg) { SAP(reg); }
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

        static void OnCollisionEvent(const CollisionEvent& e);

        CollisionLayerMatrix layerMatrix_;

        std::vector<std::pair<entt::entity, entt::entity>> broadPairs_;

        using SupportFunc = std::function<glm::vec3(const glm::vec3& dir, const ColliderComponent& col)>;
        EnumArray<ColliderType, SupportFunc> supportFunctions_;
    };
}

#endif //MANGO_COLLISIONSYSTEM_H
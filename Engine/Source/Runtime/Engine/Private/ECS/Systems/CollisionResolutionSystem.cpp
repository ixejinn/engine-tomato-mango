#include "ECS/Systems/CollisionResolutionSystem.h"
#include "ECS/Components/OnCollision.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/SystemUpdateContexts.h"
#include "Collision/CollisionConstants.h"
#include "State/State.h"
#include "SimulationConfig.h"
#include "Utils/RegistryEntry.h"
//REGISTER_SYSTEM(tomato::SystemPhase::OnCollision, CollisionResolutionSystem);

namespace tomato
{
    void CollisionResolutionSystem::Update(SimContext& simCtx)
    {
        constexpr float CORRECTION_SPEED = 3.5f;
        constexpr float EPSILON = 0.001f;

        auto& registry = simCtx.state->GetRegistry();

        auto penetrationView = registry.view<OnPenetrationComponent, TransformComponent, RootEntityTag>();
        for (auto [e, pen, trf] : penetrationView.each())
        {
            auto& info = pen.info;
            if (e == pen.e1)
                trf.AddPosition(-info.normal * info.depth * info.weight * FIXED_DELTA_TIME * CORRECTION_SPEED);
            else
                trf.AddPosition(info.normal * info.depth * (1 - info.weight) * FIXED_DELTA_TIME * CORRECTION_SPEED);

            registry.remove<OnPenetrationComponent>(e);
        }

        auto collisionView = registry.view<OnCollisionComponent, TransformComponent, VelocityComponent, RootEntityTag>();
        for (auto [e, col, trf, vel] : collisionView.each())
        {
            if (col.type != Exit)
            {
                auto& info = col.info;
                glm::vec3 remainingMove = (1 - info.depth * info.weight) * vel.velocity;

                trf.AddPosition((vel.velocity * FIXED_DELTA_TIME * info.depth - info.normal * COLLISION_SKIN) * info.weight);
                vel.velocity = remainingMove - glm::dot(remainingMove, info.normal) * info.normal;

                if (-EPSILON < vel.velocity.x && vel.velocity.x < EPSILON)
                    vel.velocity.x = 0.f;
                if (-EPSILON < vel.velocity.y && vel.velocity.y < EPSILON)
                    vel.velocity.y = 0.f;
                if (-EPSILON < vel.velocity.z && vel.velocity.z < EPSILON)
                    vel.velocity.z = 0.f;
            }

            registry.remove<OnCollisionComponent>(e);
        }
    }
}
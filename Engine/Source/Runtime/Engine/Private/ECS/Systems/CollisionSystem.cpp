#include <glm/vec3.hpp>
#include <entt/entt.hpp>
#include "ECS/Systems/CollisionSystem.h"
#include "ECS/Components/ComponentsPhys.h"
#include "ECS/Components/Character.h"
#include "ECS/Entity/Hierarchy.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "Collision/CollisionEvent.h"
#include "Collision/Broad/SAP.h"
#include "Collision/Narrow/GJK/GJK.h"
#include "Simulation/SimulationConfig.h"
#include "Event/EventDispatcher.h"
#include "GameObject/Character/CharacterMovement.h"
#include "Utils/Logger.h"

namespace tomato
{
    CollisionSystem::CollisionSystem()
    : broadPhase_(std::make_unique<SAP>())
    , narrowPhase_(std::make_unique<GJK>())
    {
        EventDispatcher::GetInstance().Connect<PenetrationEvent, &CollisionSystem::OnPenetration>();

        EventDispatcher::GetInstance().Connect<ChangeMovementModeEvent, &CharacterMovement::ChangeMovementMode>();
    }

    CollisionSystem::~CollisionSystem() = default;

    void CollisionSystem::Update(SimContext& simCtx)
    {
        RunBroadPhase(simCtx);
        RunNarrowPhase(simCtx);

        EventDispatcher::GetInstance().Update<PenetrationEvent>();
        ResolveCollision(simCtx.state->GetRegistry());

        EventDispatcher::GetInstance().Update<ChangeMovementModeEvent>();
    }

    void CollisionSystem::RunBroadPhase(SimContext& simCtx)
    {
        auto& registry = simCtx.state->GetRegistry();

        UpdateAABB(registry);

        candidates_.clear();
        broadPhase_->FindCollisionCandidates(registry, candidates_);
    }

    void CollisionSystem::RunNarrowPhase(SimContext& simCtx)
    {
        events_.clear();

        auto& registry = simCtx.state->GetRegistry();
        auto& collisionPairs = registry.ctx().get<CollisionContext>().pairs;

        auto& eventDispatcher = EventDispatcher::GetInstance();

        for (const auto& candidate : candidates_)
        {
            if (!registry.valid(candidate.a) || !registry.valid(candidate.b))
                continue;

            auto& col1 = registry.get<ColliderComponent>(candidate.a);
            auto& col2 = registry.get<ColliderComponent>(candidate.b);

            if (auto result = narrowPhase_->EvaluateCollision(registry, candidate.a, candidate.b)) {
                // std::cout << "     COLLISION: " << (int)candidate.a << " " << (int)candidate.b << "\n";
                // Collision detected
                if (!collisionPairs.contains(candidate))
                {
                    collisionPairs[candidate].normal = result->normal;

                    // Enter
                    if (col1.isTrigger || col2.isTrigger)
                    {
                        std::cout << "          enter " << (int)candidate.a << " " << (int)candidate.b << "\n";
                        eventDispatcher.Enqueue(TriggerEnterEvent{candidate.a, candidate.b, &registry});

                        if (registry.all_of<CharacterTag>(GetRootEntity(registry, candidate.a)))
                            eventDispatcher.Enqueue(ChangeMovementModeEvent{candidate.a, simCtx.state, Walking});
                        if (registry.all_of<CharacterTag>(GetRootEntity(registry, candidate.b)))
                            eventDispatcher.Enqueue(ChangeMovementModeEvent{candidate.b, simCtx.state, Walking});
                    }
                    else
                    {
                        std::cout << "          enter " << (int)candidate.a << " " << (int)candidate.b << "\n";
                        events_.emplace_back(candidate.a, candidate.b, result.value());
                        eventDispatcher.Enqueue(CollisionEnterEvent{candidate.a, candidate.b, &registry, result.value()});
                    }
                }
                else
                {
                    if (result->skin)
                        result->normal = collisionPairs[candidate].normal;
                    else
                        collisionPairs[candidate].normal = result->normal;

                    // Stay
                    if (col1.isTrigger || col2.isTrigger)
                    {
                        //std::cout << "          stay " << collisionPairs.size() << "\n";
                        eventDispatcher.Enqueue(TriggerStayEvent{candidate.a, candidate.b, &registry});
                    }
                    else
                    {
                        events_.emplace_back(candidate.a, candidate.b, result.value());
                        eventDispatcher.Enqueue(CollisionStayEvent{candidate.a, candidate.b, &registry, result.value()});
                    }
                }

                collisionPairs[candidate].curr = 2;
            }
        }

        for (auto it = collisionPairs.begin(); it != collisionPairs.end(); )
        {
            if (it->second.curr <= 0)
            {
                // Exit
                auto* col1 = registry.try_get<ColliderComponent>(it->first.a);
                auto* col2 = registry.try_get<ColliderComponent>(it->first.b);

                if (col1 && col2)
                {
                    std::cout << "          exit " << (int)it->first.a << " " << (int)it->first.b << "\n";
                    if (col1->isTrigger || col2->isTrigger)
                    {
                        //std::cout << "          exit " << (int)it->first.a << " " << (int)it->first.b << "\n";
                        EventDispatcher::GetInstance().Enqueue(TriggerExitEvent{ it->first.a, it->first.b, &registry });

                        if (registry.all_of<CharacterTag>(GetRootEntity(registry, it->first.a)))
                            eventDispatcher.Enqueue(ChangeMovementModeEvent{ it->first.a, simCtx.state, Falling });
                        if (registry.all_of<CharacterTag>(GetRootEntity(registry, it->first.b)))
                            eventDispatcher.Enqueue(ChangeMovementModeEvent{ it->first.b, simCtx.state, Falling });
                    }
                    else
                    {
                        EventDispatcher::GetInstance().Enqueue(CollisionExitEvent{ it->first.a, it->first.b, &registry });
                    }
                }

                it = collisionPairs.erase(it);
            }
            else
            {
                --it->second.curr;
                ++it;
            }
        }
    }

    void CollisionSystem::UpdateAABB(entt::registry& reg)
    {
        auto view = reg.view<ColliderComponent, TransformComponent>();
        for (auto [e, col, trf] : view.each())
        {
            if (!col.aabbDirty)
                continue;

            glm::vec3 wPos = trf.GetWorldPosition();
            // Sweep AABB
            if (const auto velPtr = reg.try_get<VelocityComponent>(GetRootEntity(reg, e)))
                wPos += velPtr->velocity * FIXED_DELTA_TIME;

            auto halfExtents = trf.GetWorldScale() * 0.5f;
            if (col.type == ColliderType::Sphere)
            {
                const glm::vec3 radius{halfExtents.x};

                col.max = wPos + radius + HALF_COLLISION_SKIN;
                col.min = wPos - radius - HALF_COLLISION_SKIN;
            }
            else
            {
                auto R = glm::toMat4(trf.GetWorldQuaternion());

                glm::vec3 aabbHalfExtents
                {
                    glm::abs(R[0][0]) * halfExtents.x + glm::abs(R[1][0]) * halfExtents.y + glm::abs(R[2][0]) * halfExtents.z,
                    glm::abs(R[0][1]) * halfExtents.x + glm::abs(R[1][1]) * halfExtents.y + glm::abs(R[2][1]) * halfExtents.z,
                    glm::abs(R[0][2]) * halfExtents.x + glm::abs(R[1][2]) * halfExtents.y + glm::abs(R[2][2]) * halfExtents.z
                };

                col.max = wPos + aabbHalfExtents + HALF_COLLISION_SKIN;
                col.min = wPos - aabbHalfExtents - HALF_COLLISION_SKIN;
            }
        }
    }

    void CollisionSystem::ResolveContact(entt::registry& reg, entt::entity e1, entt::entity e2, const CollisionInfo& info)
    {
        entt::entity root1 = GetRootEntity(reg, e1);
        entt::entity root2 = GetRootEntity(reg, e2);

        auto& trfRoot1 = reg.get<TransformComponent>(root1);
        auto& trfRoot2 = reg.get<TransformComponent>(root2);

        std::cout << " ===== SOLVE COLLISION " << (int)e1 << " " << (int)e2 << "\n";
        std::cout << "       normal: " << info.normal.x << " " << info.normal.y << " " << info.normal.z << "\n";

        glm::vec3 v1{ 0.f };
        glm::vec3 v2{ 0.f };

        auto vel1 = reg.try_get<VelocityComponent>(root1);
        if (vel1)
            v1 = vel1->velocity;

        auto vel2 = reg.try_get<VelocityComponent>(root2);
        if (vel2)
            v2 = vel2->velocity;

        float lenV1 = glm::length(v1);
        float lenV2 = glm::length(v2);
        float sumV = lenV1 + lenV2;

        float weight = 0.5f;
        if (sumV >= 1e-6f)
            weight = lenV1 / sumV;
        std::cout << "       weight: " << weight << "\n";

        constexpr float epsilon = 1e-4f;

        if (vel1)
        {
            auto pos = trfRoot1.GetLocalPosition();
            std::cout << "         position 1: " << pos.x << " " << pos.y << " " << pos.z << " " << trfRoot1.GetLocalEulerDegree().y << "\n";
            //auto rot = trfRoot1.GetLocalEulerDegree();
            //std::cout << "         rotation 1: " << rot.x << " " << rot.y << " " << rot.z << " " << "\n";
            std::cout << "         velocity 1: " << vel1->velocity.x << " " << vel1->velocity.y << " " << vel1->velocity.z << "\n";

            if (!info.skin)
            {
                trfRoot1.AddPosition((vel1->velocity * FIXED_DELTA_TIME * info.depth - info.normal * COLLISION_SKIN) * weight);
                pos = trfRoot1.GetLocalPosition();
                std::cout << "         position 2: " << pos.x << " " << pos.y << " " << pos.z << "\n";

                glm::vec3 remainingMove = (1 - info.depth * weight) * vel1->velocity;
                vel1->velocity = remainingMove - glm::dot(remainingMove, info.normal) * info.normal;
            }
            else
            {
                // COLLISION_SKIN 만큼만 떨어져 있어서 GJK Distance로부터 받은 충돌 정보
                trfRoot1.AddPosition(-info.normal * (COLLISION_SKIN - info.depth) * weight);
                pos = trfRoot1.GetLocalPosition();
                std::cout << "         position S: " << pos.x << " " << pos.y << " " << pos.z << "\n";

                vel1->velocity -= glm::dot(vel1->velocity, info.normal) * info.normal;
            }

            if (-epsilon < vel1->velocity.x && vel1->velocity.x < epsilon)
                vel1->velocity.x = 0.f;
            if (-epsilon < vel1->velocity.y && vel1->velocity.y < epsilon)
                vel1->velocity.y = 0.f;
            if (-epsilon < vel1->velocity.z && vel1->velocity.z < epsilon)
                vel1->velocity.z = 0.f;

            std::cout << "         velocity _: " << vel1->velocity.x << " " << vel1->velocity.y << " " << vel1->velocity.z << "\n";
        }

        if (vel2)
        {
            auto pos = trfRoot2.GetLocalPosition();
            std::cout << "         position 1: " << pos.x << " " << pos.y << " " << pos.z << "\n";
            std::cout << "         velocity 1: " << vel2->velocity.x << " " << vel2->velocity.y << " " << vel2->velocity.z << "\n";

            if (!info.skin)
            {
                trfRoot2.AddPosition((vel2->velocity * FIXED_DELTA_TIME * info.depth + info.normal * COLLISION_SKIN) * weight);
                pos = trfRoot2.GetLocalPosition();
                std::cout << "         position 2: " << pos.x << " " << pos.y << " " << pos.z << "\n";

                glm::vec3 remainingMove = (1 - info.depth * weight) * vel2->velocity;
                vel2->velocity = remainingMove + glm::dot(remainingMove, -info.normal) * info.normal;
            }
            else
            {
                // COLLISION_SKIN 만큼만 떨어져 있어서 GJK Distance로부터 받은 충돌 정보
                trfRoot2.AddPosition(info.normal * (COLLISION_SKIN - info.depth) * weight);
                pos = trfRoot2.GetLocalPosition();
                std::cout << "         position S: " << pos.x << " " << pos.y << " " << pos.z << "\n";

                vel2->velocity += glm::dot(vel2->velocity, -info.normal) * info.normal;
            }

            if (-epsilon < vel2->velocity.x && vel2->velocity.x < epsilon)
                vel2->velocity.x = 0.f;
            if (-epsilon < vel2->velocity.y && vel2->velocity.y < epsilon)
                vel2->velocity.y = 0.f;
            if (-epsilon < vel2->velocity.z && vel2->velocity.z < epsilon)
                vel2->velocity.z = 0.f;

            std::cout << "         velocity _: " << vel2->velocity.x << " " << vel2->velocity.y << " " << vel2->velocity.z << "\n";
        }
    }

    void CollisionSystem::OnPenetration(const PenetrationEvent& e)
    {
        std::cout << " ===== SOLVE PENETRATION " << (int)e.e1 << " " << (int)e.e2 << "\n";
        std::cout << "       normal: " << e.info.normal.x << " " << e.info.normal.y << " " << e.info.normal.z << "\n";
        entt::entity root1 = GetRootEntity(*e.reg, e.e1);
        entt::entity root2 = GetRootEntity(*e.reg, e.e2);

        auto& trfRoot1 = e.reg->get<TransformComponent>(root1);
        auto& trfRoot2 = e.reg->get<TransformComponent>(root2);

        //constexpr float CORRECTION_SPEED = 3.5f;
        constexpr float CORRECTION_SPEED = 5.f;

        // auto bef = trfRoot1.GetLocalPosition();
        trfRoot1.AddPosition(-e.info.normal * e.info.depth * e.info.weight * FIXED_DELTA_TIME * CORRECTION_SPEED);
        // auto aft = trfRoot1.GetLocalPosition();
        // TMT_INFO << (int)root1 << " bef: " << bef.x << " " << bef.y << " " << bef.z;
        // TMT_INFO << (int)root1 << " aft: " << aft.x << " " << aft.y << " " << aft.z;

        // bef = trfRoot2.GetLocalPosition();
        trfRoot2.AddPosition(e.info.normal * e.info.depth * (1 - e.info.weight) * FIXED_DELTA_TIME * CORRECTION_SPEED);
        // aft = trfRoot2.GetLocalPosition();
        // TMT_INFO << (int)root2 << " bef: " << bef.x << " " << bef.y << " " << bef.z;
        // TMT_INFO << (int)root2 << " aft: " << aft.x << " " << aft.y << " " << aft.z;
    }

    void CollisionSystem::ResolveCollision(entt::registry& reg)
    {
        for (auto& event : events_)
        {
            ResolveContact(reg, event.e1, event.e2, event.info);
        }
    }
}

#include <glm/vec3.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <entt/entt.hpp>
#include "ECS/Systems/CollisionSystem.h"
#include "ECS/Components/ComponentsPhys.h"
#include "ECS/Components/Movement.h"
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
        auto& eventDispatcher = EventDispatcher::GetInstance();
        eventDispatcher.Connect<PenetrationEvent, &CollisionSystem::CorrectPenetration>();
        eventDispatcher.Connect<ChangeMovementModeEvent, &CharacterMovement::ChangeMovementMode>();
    }

    CollisionSystem::~CollisionSystem() = default;

    void CollisionSystem::Update(SimContext& simCtx)
    {
        RunBroadPhase(simCtx);
        RunNarrowPhase(simCtx);

        EventDispatcher::GetInstance().Update<PenetrationEvent>();
        ResolveContacts(simCtx.state->GetRegistry());

        EventDispatcher::GetInstance().Update<ChangeMovementModeEvent>();
    }

    void CollisionSystem::RunBroadPhase(SimContext& simCtx)
    {
        candidates_.clear();

        auto& registry = simCtx.state->GetRegistry();

        UpdateAABB(registry);
        broadPhase_->FindContactPairCandidates(registry, candidates_);
    }

    void CollisionSystem::RunNarrowPhase(SimContext& simCtx)
    {
        contacts_.clear();

        auto& registry = simCtx.state->GetRegistry();
        auto& contactPairs = registry.ctx().get<CollisionContext>().pairs;
        auto& eventDispatcher = EventDispatcher::GetInstance();

        // Check contact pair candidates
        for (const auto& candidate : candidates_)
        {
            if (!registry.valid(candidate.a) || !registry.valid(candidate.b))
                continue;

            if (auto result = narrowPhase_->EvaluateContactPair(registry, candidate))
            {
                // Collision detected
                if (!contactPairs.contains(candidate))
                {
                    contactPairs[candidate].normal = result->normal;

                    // Enter
                    if (result->trigger)
                    {
                        std::cout << "      trg ENTER " << candidate << "\n";
                        eventDispatcher.Enqueue(TriggerEnterEvent{candidate.a, candidate.b, &registry});
                    }
                    else
                    {
                        std::cout << "      col ENTER " << candidate << "\n";
                        eventDispatcher.Enqueue(CollisionEnterEvent{candidate.a, candidate.b, &registry, result.value()});
                        contacts_.emplace_back(candidate.a, candidate.b, result.value());
                    }
                }
                else
                {
                    if (result->distance < COLLISION_SKIN + 1e-4f)
                        result->normal = contactPairs[candidate].normal;
                    else
                        contactPairs[candidate].normal = result->normal;

                    // Stay
                    if (result->trigger)
                    {
                        // std::cout << "      trg STAY " << candidate << "\n";
                        eventDispatcher.Enqueue(TriggerStayEvent{candidate.a, candidate.b, &registry});
                    }
                    else
                    {
                        // std::cout << "      col STAY " << candidate << "\n";
                        eventDispatcher.Enqueue(CollisionStayEvent{candidate.a, candidate.b, &registry, result.value()});
                        contacts_.emplace_back(candidate.a, candidate.b, result.value());
                    }
                }

                contactPairs[candidate].exitCnt = EXIT_CNT;
            }
        }

        // Check exit of contact pairs
        for (auto it = contactPairs.begin(); it != contactPairs.end(); )
        {
            if (it->second.exitCnt <= 0)
            {
                // Exit
                auto* col1 = registry.try_get<ColliderComponent>(it->first.a);
                auto* col2 = registry.try_get<ColliderComponent>(it->first.b);

                if (col1 && col2)
                {
                    if (col1->isTrigger || col2->isTrigger)
                    {
                        std::cout << "      trg EXIT " << it->first << "\n";
                        EventDispatcher::GetInstance().Enqueue(TriggerExitEvent{ it->first.a, it->first.b, &registry });
                    }
                    else
                    {
                        std::cout << "      col EXIT " << it->first << "\n";
                        EventDispatcher::GetInstance().Enqueue(CollisionExitEvent{ it->first.a, it->first.b, &registry });
                    }
                }

                it = contactPairs.erase(it);
            }
            else
            {
                --it->second.exitCnt;
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
            col.aabbDirty = false;

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

    void CollisionSystem::ResolveContacts(entt::registry& reg)
    {
        for (auto& contact : contacts_)
            ResolveContact(reg, contact.e1, contact.e2, contact.info);
    }

    void CollisionSystem::ResolveContact(entt::registry& reg, entt::entity e1, entt::entity e2, const ContactData& data)
    {
        std::cout << " ===== SOLVE COLLISION " << (int)e1 << " " << (int)e2 << "\n";
        std::cout << "       normal: " << data.normal.x << " " << data.normal.y << " " << data.normal.z << "\n";

        entt::entity root1 = GetRootEntity(reg, e1);
        entt::entity root2 = GetRootEntity(reg, e2);

        glm::vec3 v1{0.f};
        glm::vec3 v2{0.f};

        auto vel1 = reg.try_get<VelocityComponent>(root1);
        if (vel1)
            v1 = vel1->velocity;

        auto vel2 = reg.try_get<VelocityComponent>(root2);
        if (vel2)
            v2 = vel2->velocity;

        float lenV1 = glm::length(v1);
        float lenV2 = glm::length(v2);
        float sumV = lenV1 + lenV2;

        float weight1 = 0.5f;
        if (sumV >= 1e-6f)
            weight1 = lenV1 / sumV;
        std::cout << "       weight1: " << weight1 << "\n";
        float weight2 = 1 - weight1;

        auto& trfRoot1 = reg.get<TransformComponent>(root1);
        auto& trfRoot2 = reg.get<TransformComponent>(root2);

        constexpr float epsilon = 1e-4f;

        if (data.hitTime.has_value())
        {
            // CCD contact data
            const float ht = data.hitTime.value();
            std::cout << "       hit time: " << ht << "\n";

            if (vel1)
            {
                std::cout << "         [" << (int)e1 << "]\n";
                std::cout << "          position 1: " << glm::to_string(trfRoot1.GetLocalPosition()) << "\n";

                trfRoot1.AddPosition((vel1->velocity * FIXED_DELTA_TIME * ht - data.normal * COLLISION_SKIN) * weight1);
                std::cout << "          position C: " << glm::to_string(trfRoot1.GetLocalPosition()) << "\n";

                glm::vec3 remainingMove = (1 - ht * weight1) * vel1->velocity;
                vel1->velocity = remainingMove - glm::dot(remainingMove, data.normal) * data.normal;

                if (-epsilon < vel1->velocity.x && vel1->velocity.x < epsilon)
                    vel1->velocity.x = 0.f;
                if (-epsilon < vel1->velocity.y && vel1->velocity.y < epsilon)
                    vel1->velocity.y = 0.f;
                if (-epsilon < vel1->velocity.z && vel1->velocity.z < epsilon)
                    vel1->velocity.z = 0.f;
                std::cout << "          velocity C: " << glm::to_string(vel1->velocity) << "\n";
            }

            if (vel2)
            {
                std::cout << "         [" << (int)e2 << "]\n";
                std::cout << "          position 1: " << glm::to_string(trfRoot2.GetLocalPosition()) << "\n";

                trfRoot2.AddPosition((vel2->velocity * FIXED_DELTA_TIME * ht + data.normal * COLLISION_SKIN) * weight2);
                std::cout << "          position C: " << glm::to_string(trfRoot2.GetLocalPosition()) << "\n";

                glm::vec3 remainingMove = (1 - ht * weight2) * vel2->velocity;
                vel2->velocity = remainingMove + glm::dot(remainingMove, -data.normal) * data.normal;

                if (-epsilon < vel2->velocity.x && vel2->velocity.x < epsilon)
                    vel2->velocity.x = 0.f;
                if (-epsilon < vel2->velocity.y && vel2->velocity.y < epsilon)
                    vel2->velocity.y = 0.f;
                if (-epsilon < vel2->velocity.z && vel2->velocity.z < epsilon)
                    vel2->velocity.z = 0.f;
                std::cout << "          velocity C: " << glm::to_string(vel2->velocity) << "\n";
            }
        }
        else if (data.distance >= 0)
        {
            // COLLISION_SKIN 만큼만 떨어져 있어서 GJK Distance로부터 받은 충돌 정보
            std::cout << "       distance: " << data.distance << "\n";

            if (vel1)
            {
                std::cout << "         [" << (int)e1 << "]\n";
                std::cout << "         position 1: " << glm::to_string(trfRoot1.GetLocalPosition()) << "\n";

                trfRoot1.AddPosition(-data.normal * (COLLISION_SKIN - data.distance) * weight1);
                std::cout << "         position -: " << glm::to_string(trfRoot1.GetLocalPosition()) << "\n";

                vel1->velocity -= glm::dot(vel1->velocity, data.normal) * data.normal;

                if (-epsilon < vel1->velocity.x && vel1->velocity.x < epsilon)
                    vel1->velocity.x = 0.f;
                if (-epsilon < vel1->velocity.y && vel1->velocity.y < epsilon)
                    vel1->velocity.y = 0.f;
                if (-epsilon < vel1->velocity.z && vel1->velocity.z < epsilon)
                    vel1->velocity.z = 0.f;
                std::cout << "         velocity -: " << glm::to_string(vel1->velocity) << "\n";
            }

            if (vel2)
            {
                std::cout << "         [" << (int)e2 << "]\n";
                std::cout << "          position 1: " << glm::to_string(trfRoot2.GetLocalPosition()) << "\n";

                trfRoot2.AddPosition(data.normal * (COLLISION_SKIN - data.distance) * weight2);
                std::cout << "         position -: " << glm::to_string(trfRoot2.GetLocalPosition()) << "\n";

                vel2->velocity += glm::dot(vel2->velocity, -data.normal) * data.normal;

                if (-epsilon < vel2->velocity.x && vel2->velocity.x < epsilon)
                    vel2->velocity.x = 0.f;
                if (-epsilon < vel2->velocity.y && vel2->velocity.y < epsilon)
                    vel2->velocity.y = 0.f;
                if (-epsilon < vel2->velocity.z && vel2->velocity.z < epsilon)
                    vel2->velocity.z = 0.f;
                std::cout << "         velocity -: " << glm::to_string(vel2->velocity) << "\n";
            }
        }
        else
        {
            // 겹침 보정

            std::cout << "         [" << (int)e1 << "]\n";
            std::cout << "         position 1: " << glm::to_string(trfRoot1.GetLocalPosition()) << "\n";
            trfRoot1.AddPosition(-data.normal * -data.distance * weight1 * FIXED_DELTA_TIME * CORRECTION_SPEED);
            std::cout << "         position P: " << glm::to_string(trfRoot1.GetLocalPosition()) << "\n";

            std::cout << "         [" << (int)e2 << "]\n";
            std::cout << "         position 1: " << glm::to_string(trfRoot2.GetLocalPosition()) << "\n";
            trfRoot2.AddPosition(data.normal * -data.distance * weight2 * FIXED_DELTA_TIME * CORRECTION_SPEED);
            std::cout << "         position P: " << glm::to_string(trfRoot2.GetLocalPosition()) << "\n";
        }
    }

    void CollisionSystem::CorrectPenetration(const PenetrationEvent& e)
    {
        std::cout << " ===== SOLVE PENETRATION " << (int)e.e1 << " " << (int)e.e2 << "\n";
        std::cout << "       normal: " << e.info.normal.x << " " << e.info.normal.y << " " << e.info.normal.z << "\n";
        entt::entity root1 = GetRootEntity(*e.reg, e.e1);
        entt::entity root2 = GetRootEntity(*e.reg, e.e2);

        glm::vec3 v1{0.f};
        glm::vec3 v2{0.f};

        auto vel1 = e.reg->try_get<VelocityComponent>(root1);
        if (vel1)
            v1 = vel1->velocity;

        auto vel2 = e.reg->try_get<VelocityComponent>(root2);
        if (vel2)
            v2 = vel2->velocity;

        float lenV1 = glm::length(v1);
        float lenV2 = glm::length(v2);
        float sumV = lenV1 + lenV2;

        float weight = 0.5f;
        if (sumV >= 1e-6f)
            weight = lenV1 / sumV;
        std::cout << "       weight: " << weight << "\n";

        auto& trfRoot1 = e.reg->get<TransformComponent>(root1);
        auto& trfRoot2 = e.reg->get<TransformComponent>(root2);

        std::cout << "         position 1- " << (int)e.e1 << ": " << glm::to_string(trfRoot1.GetLocalPosition()) << "\n";
        trfRoot1.AddPosition(-e.info.normal * e.info.distance * weight * FIXED_DELTA_TIME * CORRECTION_SPEED);
        std::cout << "         position 2- " << (int)e.e1 << ": " << glm::to_string(trfRoot1.GetLocalPosition()) << "\n";

        std::cout << "         position 1- " << (int)e.e2 << ": " << glm::to_string(trfRoot2.GetLocalPosition()) << "\n";
        trfRoot2.AddPosition(e.info.normal * e.info.distance * (1 - weight) * FIXED_DELTA_TIME * CORRECTION_SPEED);
        std::cout << "         position 2- " << (int)e.e2 << ": " << glm::to_string(trfRoot2.GetLocalPosition()) << "\n";
    }
}

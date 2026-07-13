#include <random>
#include <entt/entt.hpp>
#include "Particle/ParticleEmitterPool.h"
#include "ECS/Components/Nametag.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Particle.h"
#include "ECS/Components/Visibility.h"
#include "ECS/Entity/Hierarchy.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Render/Texture.h"

namespace tomato
{
    ParticleEmitterPool::ParticleEmitterPool(const PassKey<State>& key, entt::registry& reg, uint8_t poolSize)
        : registry_(reg)
    {
        freeEmitters_.reserve(poolSize);

        for (int i = 0; i < poolSize; ++i)
        {
            entt::entity e = registry_.create();
            registry_.emplace<NametagComponent>(e,
                GenerateUUID(), GenerateEntityName(registry_, "ParticleEffect"));
            registry_.emplace<TransformComponent>(e);
            registry_.emplace<RootEntityTag>(e);
            registry_.emplace<VisibilityComponent>(e);

            auto& particle = registry_.emplace<ParticleEffectComponent>(e);
            particle.positions.reserve(ParticleEffectComponent::MAX_PARTICLE);
            particle.velocities.reserve(ParticleEffectComponent::MAX_PARTICLE);
            particle.lifetimes.reserve(ParticleEffectComponent::MAX_PARTICLE);
            particle.scales.reserve(ParticleEffectComponent::MAX_PARTICLE);

            freeEmitters_.push_back(e);
        }
    }

    std::optional<entt::entity> ParticleEmitterPool::Acquire(ParticleEffectType type, glm::vec3 pos)
    {
        if (freeEmitters_.empty() || type >= COUNT)
        {
            TMT_DEBUG << "Empty particle emitter pool.";
            return std::nullopt;
        }

        entt::entity e = freeEmitters_.back();
        freeEmitters_.pop_back();

        auto& transform = registry_.get<TransformComponent>(e);
        transform.SetPosition(pos);

        auto& particle = registry_.get<ParticleEffectComponent>(e);
        switch (type)
        {
        case Jump:
        case Move:
        {
            const auto& details = ParticleEffectMap::ParticleDetails[Jump];
            particle.activeCnt = details.spawnNum;

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 359);

            for (int i = 0; i < particle.activeCnt; ++i)
            {
                particle.positions.emplace_back(0.f);

                auto radian = glm::radians(static_cast<float>(dis(gen)));
                particle.velocities.emplace_back(glm::sin(radian), 0, glm::cos(radian));

                particle.lifetimes.emplace_back(details.duration.first, std::chrono::steady_clock::now());
                particle.scales.push_back(details.scale.first);
            }

            particle.texture = GetAssetID(Texture::PrimitiveName);
        }
            break;

//        case Move:
//            break;
        default: ;
        }

        return e;
    }

    std::optional<entt::entity> ParticleEmitterPool::Acquire(ParticleEffectType type, entt::entity parent)
    {
        if (freeEmitters_.empty())
        {
            TMT_DEBUG << "Empty particle emitter pool.";
            return std::nullopt;
        }

        entt::entity e = freeEmitters_.back();
        freeEmitters_.pop_back();

        SetHierarchy(registry_, parent, e);

        auto& particle = registry_.get<ParticleEffectComponent>(e);
        switch (type)
        {
            case Jump:
            case Move:
            {
                const auto& details = ParticleEffectMap::ParticleDetails[Jump];
                particle.activeCnt = details.spawnNum;

                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, 359);

                for (int i = 0; i < particle.activeCnt; ++i)
                {
                    particle.positions.emplace_back(0.f);

                    auto radian = glm::radians(static_cast<float>(dis(gen)));
                    particle.velocities.emplace_back(glm::sin(radian), 0, glm::cos(radian));

                    particle.lifetimes.emplace_back(details.duration.first, std::chrono::steady_clock::now());
                    particle.scales.push_back(details.scale.first);
                }

                particle.texture = GetAssetID(Texture::PrimitiveName);
            }
            break;
            default: ;
        }

        return e;
    }

    bool ParticleEmitterPool::Release(entt::entity e)
    {
        auto* particle = registry_.try_get<ParticleEffectComponent>(e);
        if (!particle)
            return false;

        SetHierarchy(registry_, entt::null, e);

        particle->positions.clear();
        particle->velocities.clear();
        particle->lifetimes.clear();
        particle->scales.clear();
        particle->activeCnt = 0;

        freeEmitters_.push_back(e);
        return true;
    }

    void ParticleEmitterPool::SetParticleEmitter(ParticleEffectType type)
    {

    }
}

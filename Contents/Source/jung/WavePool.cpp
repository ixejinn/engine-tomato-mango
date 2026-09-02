#include "WavePool.h"
#include <entt/entt.hpp>

#include "ECS/Components/Nametag.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Visibility.h"
#include "ECS/Components/Lifetime.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Character.h"
#include "ECS/Components/EditorTag.h"
#include "WaveComponent.h"
#include "ECS/Entity/Entity.h"
#include "Prefab/Prefab.h"

#include "Resource/AssetRegistry.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"

#include "GameplayConfig.h"

#include "WaveColliderPool.h"

using namespace tomato;

void WavePoolTraits::Assemble(entt::registry& registry_, entt::entity wave)
{
    auto& nameTag = registry_.get<NametagComponent>(wave);
    nameTag.name = registry_.ctx().get<EntityNameGenerator>().Generate("Wave");

    auto& transform = registry_.get<TransformComponent>(wave);
    transform.SetPosition(glm::vec3{ 0, -2.9f, 0 });
    transform.SetScale(glm::vec3{ 0, 0.1f, 0 });

    registry_.emplace<RenderComponent>(wave,
        glm::vec4(1.f),
        GetAssetID("Primitive::OpenCylinder_50_10"),
        GetAssetID(Shader::PrimitiveName),
        GetAssetID(Texture::PrimitiveName));
    registry_.emplace<EditorHidden>(wave);

    auto& waveCmp = registry_.emplace<WaveComponent>(wave, false, glm::vec3{ 0, -2.9f, 0 }, 10.f, 0.01f);
    waveCmp.colliders.reserve(MAX_PLAYER_NUM - 1);

    auto& colPool = registry_.ctx().get<WaveColliderPool>();
    auto charView = registry_.view<CharacterTag>();
    for (int i = 0; i < MAX_PLAYER_NUM; i++)
    {
        //if(*(charView.begin() + i) == 
        waveCmp.colliders.push_back(colPool.Acquire(wave, *(charView.begin() + i)).value());
    }
}

void WavePoolTraits::Reset(entt::registry& registry_, entt::entity e, entt::entity owner, glm::vec3 pos, float speed, float radius)
{
    auto& waveComp = registry_.get<WaveComponent>(e);
    waveComp.active = true;
    waveComp.owner = owner;
    waveComp.origin = pos;
    waveComp.speed = speed;
    waveComp.radius = radius;
    waveComp.startTick = 0;

    auto& transform = registry_.get<TransformComponent>(e);
    transform.SetPosition(pos);
    transform.SetScale({ 0.f, 0.1f, 0.f });

    auto& visibility = registry_.get<VisibilityComponent>(e);
    visibility.visible = true;

    for (auto col : waveComp.colliders)
    {
        auto& transformComp = registry_.get<TransformComponent>(col);
        transformComp.SetPosition(waveComp.origin);
        registry_.emplace_or_replace<WaveColliderTag>(col);

        /*auto& life = registry_.get<LifetimeComponent>(col);
        life.isActive = true;*/
        //auto& hierarchy = registry_.get<HierarchyComponent>(col);
        //for (auto child : hierarchy.children)
        //{
        //    auto& life = registry_.get<LifetimeComponent>(child);
        //    life.isActive = true;
        //}
    }
}

bool WavePoolTraits::Deactivate(entt::registry& registry_, entt::entity e)
{
    auto* wave = registry_.try_get<WaveComponent>(e);
    if (!wave)
        return false;

    wave->active = false;

    auto& visibility = registry_.get<VisibilityComponent>(e);
    visibility.visible = false;

    for (auto col : wave->colliders)
    {
        if (!registry_.all_of<WaveColliderTag>(col))
            continue;

        registry_.remove<WaveColliderTag>(col); //deactive
        auto& transformComp = registry_.get<TransformComponent>(col);
        transformComp.SetPosition(glm::vec3{ 100.f, 100.f, 100.f });
        /*auto& life = registry_.get<LifetimeComponent>(col);
        life.isActive = false;*/
        //auto& hierarchy = registry_.get<HierarchyComponent>(col);
        //for (auto child : hierarchy.children)
        //{
        //    auto& life = registry_.get<LifetimeComponent>(child);
        //    life.isActive = false;
        //}
    }

    return true;
}

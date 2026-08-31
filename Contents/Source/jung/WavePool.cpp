#include "WavePool.h"
#include <entt/entt.hpp>

#include "ECS/Components/Nametag.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Visibility.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/EditorTag.h"
#include "WaveComponent.h"
#include "ECS/Entity/Entity.h"
#include "Prefab/Prefab.h"

#include "Resource/AssetRegistry.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"

#include "GameplayConfig.h"

using namespace tomato;

void WavePoolTraits::Assemble(entt::registry& registry_, entt::entity wave)
{
    auto& generator = registry_.ctx().get<EntityNameGenerator>();
    registry_.emplace<NametagComponent>(wave, GenerateUUID(), generator.Generate("Wave"));
    registry_.emplace<VisibilityComponent>(wave);
    registry_.emplace<TransformComponent>(wave, glm::vec3{ 0, -2.9f, 0 }, glm::vec3(0), glm::vec3(0, 0.1f, 0));
    registry_.emplace<RenderComponent>(wave,
        glm::vec4(1.f),
        GetAssetID("Primitive::OpenCylinder_50_10"),
        GetAssetID(Shader::PrimitiveName),
        GetAssetID(Texture::PrimitiveName));
    registry_.emplace<RootEntityTag>(wave);
    registry_.emplace<EditorHidden>(wave);
    registry_.emplace<WaveComponent>(wave, false, glm::vec3{ 0, -2.9f, 0 }, 10.f, 0.01f);
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
}

bool WavePoolTraits::Deactivate(entt::registry& registry_, entt::entity e)
{
    auto* wave = registry_.try_get<WaveComponent>(e);
    if (!wave)
        return false;

    wave->active = false;

    auto& visibility = registry_.get<VisibilityComponent>(e);
    visibility.visible = false;

    return true;
}

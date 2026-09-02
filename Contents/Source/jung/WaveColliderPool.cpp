#include "WaveColliderPool.h"
#include <entt/entt.hpp>

#include "ECS/Components/Nametag.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Target.h"
#include "ECS/Components/Visibility.h"
#include "ECS/Components/Lifetime.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/EditorTag.h"
#include "WaveComponent.h"
#include "ECS/Entity/Entity.h"
#include "Prefab/Prefab.h"

#include "Resource/AssetRegistry.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"

using namespace tomato;

void WaveColliderPoolTraits::Assemble(entt::registry& registry_, entt::entity e)
{
    auto& nameTag = registry_.get<NametagComponent>(e);
    nameTag.name = registry_.ctx().get<EntityNameGenerator>().Generate("WaveCollider");

    auto& trfPt = registry_.get<TransformComponent>(e);
    trfPt.SetPosition(glm::vec3{ 100.f, 100.f, 100.f });
    trfPt.SetScale(glm::vec3{ 1.f, 0.1f, 0.1f });

    registry_.emplace<TargetComponent>(e);
    auto& renderPt = registry_.emplace<RenderComponent>(e,
        glm::vec4{ 0.8f, 0.f, 0.8f, 1.f },
        GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Plain)),
        GetAssetID(Shader::PrimitiveName),
        GetAssetID(Texture::PrimitiveName));
    //registry_.emplace<RootEntityTag>(e);
    //registry_.emplace<VisibilityComponent>(e);
    //registry_.emplace<WaveColliderComponent>(e);
    //registry_.emplace<EditorHidden>(e);

    const entt::entity col = Prefab::AttachColliderEntity(registry_, e, true);
    //registry_.get<TransformComponent>(col).SetScale(glm::vec3{1.f, 0.1f, 0.1f});
    //registry_.get<LifetimeComponent>(col).isActive = false;
}

void WaveColliderPoolTraits::Reset(entt::registry& registry_, entt::entity e, entt::entity wave, entt::entity target)
{
    auto& targetComp = registry_.get<TargetComponent>(e);
    targetComp.target = GetUUID(registry_, target);

    auto& waveCollider = registry_.emplace<WaveColliderComponent>(e, wave);
    //auto& transformComp = registry_.get<TransformComponent>(e);
    //auto& waveComp = registry_.get<WaveComponent>(wave);
    //transformComp.SetPosition(waveComp.origin);
    
    //registry_.emplace<WaveColliderTag>(e); // active

}

bool WaveColliderPoolTraits::Deactivate(entt::registry& registry_, entt::entity e)
{
    if(!registry_.all_of<WaveColliderTag>(e))
        return false;

    registry_.remove<WaveColliderTag>(e); //deactive

    return true;
}

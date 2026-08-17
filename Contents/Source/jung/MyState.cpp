#include "MyState.h"
#include "Engine.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Audio/Audio.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "Resource/Render/ParticleEffect.h"
#include "Resource/PathManager.h"
#include "Input/InputRecorder.h"
#include "Input/InputConstants.h"
#include "Input/KeyConstants.h"
#include "Utils/Logger.h"
#include "ECS/Components/Components.h"
#include "ECS/Entity/Hierarchy.h"
#include "Collision/CollisionEvent.h"
#include "Event/EventDispatcher.h"
#include "Prefab/Prefab.h"
#include "Prefab/UIPrefab.h"
#include "Serialization/ComponentSerializer.h"
#include "ECS/Components/Nametag.h"
#include "Particle/ParticleEmitterPool.h"
#include "Utils/RegistryEntry.h"
REGISTER_STATE(MyState)

using namespace tomato;

void MyState::Init()
{
    // Camera
    Prefab::CreateCamera(registry_,
        true,
        glm::vec3(0.f, 4.f, 10.f),
        glm::vec3(-30.f, 0.f, 0.f)
    );

    entt::entity player = Prefab::CreateCharacter(registry_, Prefab::Primitive::Cube, { 1, -1, 0 });
    auto& renderp1 = registry_.get<RenderComponent>(player);
    renderp1.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere));
    renderp1.color = { 0.f, 1.f, 1.f, 1.f };
    auto& channelp = registry_.get<InputChannelComponent>(player);
    channelp.channel = 0;

    entt::entity player1 = Prefab::CreateCharacter(registry_, Prefab::Primitive::Cube, { -3, -1, -3 });
    auto& channelp1 = registry_.get<InputChannelComponent>(player1);
    channelp1.channel = 1;
    entt::entity player2 = Prefab::CreateCharacter(registry_, Prefab::Primitive::Cube, { 2, -1, 5 });
    auto& channelp2 = registry_.get<InputChannelComponent>(player2);
    channelp2.channel = 2;

    // Ground
    entt::entity ground = Prefab::CreateStaticObject(registry_, Prefab::Primitive::Cube, { 0, -3, 0 });
    auto& trfGnd = registry_.get<TransformComponent>(ground);
    trfGnd.SetScale(10, 0.1, 10);
    auto& renderGnd = registry_.get<RenderComponent>(ground);
    renderGnd.color = { 0.f, 1.f, 0.f, 1.f };

    //Projectile
    auto projectile = Prefab::CreateStaticObject(registry_, Prefab::Primitive::Cube, {-3, -2, 0});
    registry_.emplace<TargetComponent>(projectile, GetUUID(registry_, player));
    registry_.emplace<VelocityComponent>(projectile);
    registry_.emplace<LifetimeComponent>(projectile);
    auto& trfPt = registry_.get<TransformComponent>(projectile);
    trfPt.SetScale({ 0.5f, 0.5f, 0.5f });
    auto& renderPt = registry_.get<RenderComponent>(projectile);
    renderPt.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Plain));

    auto projectile1 = Prefab::CreateStaticObject(registry_, Prefab::Primitive::Cube, { -3, -0, 0 });
    registry_.emplace<TargetComponent>(projectile1, GetUUID(registry_, player1));
    registry_.emplace<VelocityComponent>(projectile1);
    registry_.emplace<LifetimeComponent>(projectile1);
    auto& trfPt1 = registry_.get<TransformComponent>(projectile1);
    trfPt1.SetScale({ 0.5f, 0.5f, 0.5f });
    auto& renderPt1 = registry_.get<RenderComponent>(projectile1);
    renderPt1.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Plain));

    auto projectile2 = Prefab::CreateStaticObject(registry_, Prefab::Primitive::Cube, { -0, -2, 0 });
    registry_.emplace<TargetComponent>(projectile2, GetUUID(registry_, player2));
    registry_.emplace<VelocityComponent>(projectile2);
    registry_.emplace<LifetimeComponent>(projectile2);
    auto& trfPt2 = registry_.get<TransformComponent>(projectile2);
    trfPt2.SetScale({ 0.5f, 0.5f, 0.5f });
    auto& renderPt2 = registry_.get<RenderComponent>(projectile2);
    renderPt2.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Plain));
}

void MyState::Update()
{
}

void MyState::Exit()
{}

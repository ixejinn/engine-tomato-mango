#include "MyState.h"
#include "Engine.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Audio/Audio.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "Resource/Render/ParticleEffect.h"
#include "Resource/PathManager.h"
#include "Input/IntentTranslator.h"
#include "Input/InputIntent.h"
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
#include "WaveComponent.h"
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


    // Ground
    entt::entity ground = Prefab::CreateStaticObject(registry_, Prefab::Primitive::Cube, { 0, -3, 0 });
    auto& trfGnd = registry_.get<TransformComponent>(ground);
    trfGnd.SetScale(10, 0.1, 10);
    auto& renderGnd = registry_.get<RenderComponent>(ground);
    renderGnd.color = { 0.f, 1.f, 0.f, 1.f };
    
    // Wave
    auto wave = Prefab::CreateStaticObject(registry_, Prefab::Primitive::Cube, { 0, -2.9f, 0 });
    registry_.emplace<WaveComponent>(wave, glm::vec3{ 0, -2.9f, 0 }, 10.f);
    auto& waveRender = registry_.get<RenderComponent>(wave);
    waveRender.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::OpenCylinder));
    auto& waveTransform = registry_.get<TransformComponent>(wave);
    waveTransform.SetScale({ 0.f, 0.1f, 0.f });

    //Projectile
    auto projectile = Prefab::CreateStaticObject(registry_, Prefab::Primitive::Cube, { 0, -2.9f, 0 });
    registry_.emplace<WaveComponent>(projectile, glm::vec3{ 0, -2.9f, 0 }, 10.f);
    registry_.emplace<TargetComponent>(projectile, GetUUID(registry_, player));
    registry_.emplace<VelocityComponent>(projectile, 0.8f);
    registry_.emplace<LifetimeComponent>(projectile);
    auto& trfPt = registry_.get<TransformComponent>(projectile);
    trfPt.SetScale({ 0.5f, 0.5f, 0.5f });
    auto& renderPt = registry_.get<RenderComponent>(projectile);
    renderPt.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Plain));
}

void MyState::Update()
{
}

void MyState::Exit()
{}

#include "PlayTest/ExampleState.h"
#include "Resource/Audio/Audio.h"
#include "Resource/Render/ParticleEffect.h"
#include "Resource/AssetRegistry.h"
#include "Resource/PathManager.h"
#include "ECS/Components/Components.h"
#include "Event/EventDispatcher.h"
#include "Prefab/Prefab.h"
#include "Particle/ParticleEmitterPool.h"
#include "Input/KeyDeviceState.h"
#include "Utils/RegistryEntry.h"
REGISTER_STATE(ExampleState)

using namespace tomato;

void ExampleState::Init()
{
    //auto sfx = Audio::Create(PathManager::ProjectSound("water-drip.mp3"), 8);
    //sfxPtr = AssetRegistry<Audio>::GetInstance().Get(sfx);

    //ParticleEffect::Create(PathManager::ProjectParticle("burst_test.tmt.ptc"));
    //ParticleEffect::Create(PathManager::ProjectParticle("ribbon_particle.tmt.ptc"));
    //ParticleEffect::Create(PathManager::ProjectParticle("jump.tmt.ptc"));

    ////////////

    //entt::entity gnd = Prefab::CreateGround(registry_);
    //auto& trfGnd = registry_.get<TransformComponent>(gnd);
    //trfGnd.SetScale(20, 0.2, 20);
    //trfGnd.SetPosition(0, -0.1, 0);
    //registry_.get<RenderComponent>(gnd).color = glm::vec4(0.5, 0.5, 0.5, 1);

    ////////////

    //cubeID = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube));
    //sphereID = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere));

    //entt::entity playerCube = Prefab::CreateCharacter(registry_, true, "playerCube");
    //registry_.get<TransformComponent>(playerCube).SetPosition(1, 2, -2);
    //registry_.get<RenderComponent>(playerCube).color = glm::vec4(196/255.f, 27/225.f, 232/225.f, 1);
    //ChangeMesh(playerCube, cubeID, true);
    //registry_.get<InputChannelComponent>(playerCube).useWASD = false;

    //entt::entity playerSphere = Prefab::CreateCharacter(registry_, true, "playerSphere");
    //registry_.get<TransformComponent>(playerSphere).SetPosition(-1, 3, -2);
    //registry_.get<RenderComponent>(playerSphere).color = glm::vec4(255/255.f, 245/255.f, 53/255.f, 1);

    ////////////

    //entt::entity cube0 = Prefab::CreateWorldObject(registry_, true, false, false, "cube0");
    //auto& trfCube0 = registry_.get<TransformComponent>(cube0);
    //trfCube0.SetPosition(2, 0.5, 3);
    //trfCube0.SetRotationDegree(0, 45, 0);
    //registry_.get<RenderComponent>(cube0).color = omoi;
    //registry_.get<PhysicsBodyComponent>(cube0).inverseMass = 0;

    //entt::entity cube1 = Prefab::CreateWorldObject(registry_, true, false, false, "cube1");
    //registry_.get<TransformComponent>(cube1).SetPosition(4, 0.5, 1);
    //registry_.get<RenderComponent>(cube1).color = karui;
    //registry_.get<PhysicsBodyComponent>(cube1).inverseMass = 10.f;

    //entt::entity sphere0 = Prefab::CreateWorldObject(registry_, true, false, false, "sphere0");
    //registry_.get<TransformComponent>(sphere0).SetPosition(-2, 0.5, 3);
    //registry_.get<RenderComponent>(sphere0).color = omoi;
    //registry_.get<PhysicsBodyComponent>(sphere0).inverseMass = 0;
    //ChangeMesh(sphere0, sphereID, false);

    //entt::entity sphere1 = Prefab::CreateWorldObject(registry_, true, false, false, "sphere1");
    //registry_.get<TransformComponent>(sphere1).SetPosition(-4, 0.5, 1);
    //registry_.get<RenderComponent>(sphere1).color = karui;
    //registry_.get<PhysicsBodyComponent>(sphere1).inverseMass = 10.f;
    //ChangeMesh(sphere1, sphereID, false);

    ////////////

    //auto& particlePool = registry_.ctx().get<ParticleEmitterPool>();
    //particlePool.Acquire(registry_, GetAssetID("Resources\\Contents\\Particle\\ribbon_particle.tmt.ptc"), playerCube);
    //particlePool.Acquire(registry_, GetAssetID("Resources\\Contents\\Particle\\burst_test.tmt.ptc"), playerSphere);

    //jumpPtcl = GetAssetID("Resources\\Contents\\Particle\\jump.tmt.ptc");

    //////////

    auto& eventDispatcher = EventDispatcher::GetInstance();
    eventDispatcher.Connect<LandingEvent, &ExampleState::CallbackLand>(*this);
}

void ExampleState::Update()
{
    static int cnt = 0;

    if (KeyDeviceState::GetInstance().IsKeyPressed(Key::O) && cnt == 0)
    {
        ++cnt;

        entt::entity e = Prefab::CreateCharacter(registry_, true, "playerSphere");
        registry_.get<TransformComponent>(e).SetPosition(-1, 3, -2);
        registry_.get<RenderComponent>(e).color = glm::vec4(255/255.f, 245/255.f, 53/255.f, 1);
        registry_.get<MovementComponent>(e).control = InputControl::Accelerate;
    }
}

void ExampleState::ChangeMesh(entt::entity e, tomato::AssetID mesh, bool character)
{
    entt::entity col = registry_.get<HierarchyComponent>(e).children[0];
    registry_.get<RenderComponent>(e).mesh = mesh;
    registry_.get<RenderComponent>(col).mesh = mesh;

    if (mesh == cubeID)
        registry_.get<ColliderComponent>(col).type = ColliderType::Cube;
    else
        registry_.get<ColliderComponent>(col).type = ColliderType::Sphere;

    if (character)
    {
        entt::entity trg = registry_.get<HierarchyComponent>(col).children[0];
        registry_.get<RenderComponent>(trg).mesh = mesh;

        if (mesh == cubeID)
            registry_.get<ColliderComponent>(trg).type = ColliderType::Cube;
        else
            registry_.get<ColliderComponent>(trg).type = ColliderType::Sphere;
    }
}

void ExampleState::CallbackLand(const LandingEvent& event)
{
    if (event.remainingJumpCount == 0)
    {
        event.reg->ctx().get<ParticleEmitterPool>().Acquire(registry_, jumpPtcl, event.position);
        sfxPtr->Start();
    }
}
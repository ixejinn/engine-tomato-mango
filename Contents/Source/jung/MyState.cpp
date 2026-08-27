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
#include "WaveCollisionComponent.h"
#include "WavePool.h"
#include "Containers/EntityPool.h"

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

    Mesh::Create(Mesh::Primitive::OpenCylinder, 40, 10);
    
    /*auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 100'000; ++i)
    {
        Prefab::CreateStaticObject(registry_);
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::cout
        << std::chrono::duration_cast<std::chrono::milliseconds>(
            end - start
        ).count()
        << " ms\n";
        */

    entt::entity player = Prefab::CreateCharacter(registry_, Prefab::Primitive::Cube, { 1, -1, 0 });
    auto& renderp1 = registry_.get<RenderComponent>(player);
    renderp1.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere));
    renderp1.color = { 0.f, 1.f, 1.f, 1.f };
    auto& channelp = registry_.get<InputChannelComponent>(player);
    channelp.channel = 0;
    registry_.emplace<WaveCollisionComponent>(player);


    // Ground
    entt::entity ground = Prefab::CreateStaticObject(registry_, Prefab::Primitive::Cube, { 0, -3, 0 });
    auto& trfGnd = registry_.get<TransformComponent>(ground);
    trfGnd.SetScale(20, 0.1, 20);
    auto& renderGnd = registry_.get<RenderComponent>(ground);
    //renderGnd.color = { 0.639f, 0.8f, 0.639f, 1.f };
    renderGnd.color = { 0.710f, 0.839f, 0.573f, 1.f };
    
    //// Wave
    //auto wave = registry_.create();
    //auto& generator = registry_.ctx().get<EntityNameGenerator>();
    //registry_.emplace<NametagComponent>(wave, GenerateUUID(), generator.Generate("wave"));
    //registry_.emplace<TransformComponent>(wave, glm::vec3{ 0, -2.9f, 0 }, glm::vec3(0), glm::vec3(0, 0.1f, 0));
    //registry_.emplace<RenderComponent>(wave,
    //                                glm::vec4(1.f),
    //                                GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::OpenCylinder)),
    //                                GetAssetID(Shader::PrimitiveName),
    //                                GetAssetID(Texture::PrimitiveName));
    //registry_.emplace<VisibilityComponent>(wave);
    //registry_.emplace<WaveComponent>(wave, glm::vec3{ 0, -2.9f, 0 }, 10.f, 0.01f);
    //registry_.emplace<RootEntityTag>(wave);

    ////Projectile
    //auto projectile = Prefab::CreateStaticObject(registry_, Prefab::Primitive::Cube, { 0, 0.f, 0 });
    //registry_.emplace<TargetComponent>(projectile, GetUUID(registry_, player));
    ////registry_.emplace<LifetimeComponent>(projectile);
    //auto& trfPt = registry_.get<TransformComponent>(projectile);
    //trfPt.SetScale({ 0.5f, 0.1f, 0.5f });
    //auto& renderPt = registry_.get<RenderComponent>(projectile);
    //renderPt.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Plain));
    //renderPt.color = glm::vec4{ 0.8f, 0.f, 0.8f, 1.f };
    ////SetHierarchy(registry_, wave, projectile);
    //registry_.get<ColliderComponent>(registry_.get<HierarchyComponent>(projectile).children[0]).isTrigger = true;
    //registry_.emplace<WaveColliderTag>(projectile);

    //auto& collider = registry_.get<HierarchyComponent>(projectile).children[0];
    //registry_.get<TransformComponent>(collider).SetScale(glm::vec3{1.f, 0.1f, 0.1f});

    //WavePool wavePool(key_, registry_);
    registry_.ctx().emplace<WavePool>(key_, registry_);

    EventDispatcher::GetInstance().Connect<TriggerEnterEvent, &WaveCollisionEnter>();
    EventDispatcher::GetInstance().Connect<TriggerExitEvent, &WaveCollisionExit>();
    EventDispatcher::GetInstance().Connect<LandingEvent, MyState::MakeWaveJump>();
}

void MyState::Update()
{
}

void MyState::Exit()
{}

void MyState::WaveCollisionEnter(const tomato::TriggerEnterEvent& event) {
    entt::entity root1 = GetRootEntity(event.reg, event.e1);
    entt::entity root2 = GetRootEntity(event.reg, event.e2);

    {
        auto* testComp = event.reg->try_get<WaveCollisionComponent>(root1);
        //auto* target = event.reg->try_get<WaveColliderTag>(root2);
        if (testComp && event.reg->all_of<WaveColliderTag>(root2))
        {
            if (auto* render = event.reg->try_get<RenderComponent>(root1))
            {
                if (!testComp->color.has_value())
                    testComp->color = render->color;
                render->color = WaveCollisionComponent::COLLISION_COLOR;
            }
        }
    }

    {
        auto* testComp = event.reg->try_get<WaveCollisionComponent>(root2);
        //auto* target = event.reg->try_get<WaveColliderTag>(root1);
        if (testComp && event.reg->all_of<WaveColliderTag>(root1))
        {
            if (auto* render = event.reg->try_get<RenderComponent>(root2))
            {
                if (!testComp->color.has_value())
                    testComp->color = render->color;
                render->color = WaveCollisionComponent::COLLISION_COLOR;
            }
        }
    }
}

void MyState::WaveCollisionExit(const tomato::TriggerExitEvent& event)
{
    entt::entity root = GetRootEntity(event.reg, event.e1);
    if (auto* testComp = event.reg->try_get<WaveCollisionComponent>(root))
    {
        if (auto* render = event.reg->try_get<RenderComponent>(root))
        {
            if (testComp->color.has_value())
                render->color = testComp->color.value();
        }
    }

    root = GetRootEntity(event.reg, event.e2);
    if (auto* testComp = event.reg->try_get<WaveCollisionComponent>(root))
    {
        if (auto* render = event.reg->try_get<RenderComponent>(root))
        {
            if (testComp->color.has_value())
                render->color = testComp->color.value();
        }
    }
}

void MyState::MakeWaveJump(const LandingEvent& event)
{
    auto e = event.reg->ctx().get<WavePool>().Acquire(event.e, event.position, 0.025f);
    std::cout << "Make Wave " << (int)e.value() << "\n";
}
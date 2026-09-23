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
#include "Containers/EntityPool.h"

#include "WaveComponent.h"
#include "WaveCollisionComponent.h"
#include "WavePool.h"
#include "WaveColliderPool.h"
#include "WaveManager.h"

REGISTER_STATE(MyState)

using namespace tomato;

MyState::MyState(Engine& engine) : State(engine) {}

MyState::~MyState() = default;

void MyState::Init()
{
    // Camera
    Prefab::CreateCamera(registry_, true, true,
        "Camera",
        glm::vec3(0.f, 4.f, 10.f),
        glm::vec3(-30.f, 0.f, 0.f)
    );

    Mesh::Create(Mesh::Primitive::OpenCylinder, 50, 10);
    
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
    engine_.SetRollbackComponent<MovementComponent>();
    engine_.SetRollbackComponent<VelocityComponent>();

    entt::entity player = Prefab::CreateCharacter(registry_, true, "Player");
    auto& trfP0 = registry_.get<TransformComponent>(player);
    trfP0.SetPosition(1, -1, 0);

    auto& renderp0 = registry_.get<RenderComponent>(player);
    renderp0.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere));
    renderp0.color = { 0.f, 1.f, 1.f, 1.f };
    auto& channelp = registry_.get<InputChannelComponent>(player);
    channelp.channel = 0;
    registry_.emplace<WaveCollisionComponent>(player);
    entt::entity colObj = registry_.get<HierarchyComponent>(player).children[0];
    auto& colp = registry_.get<ColliderComponent>(colObj);
    colp.layer = CollisionLayer::Wave1;

    entt::entity player1 = Prefab::CreateCharacter(registry_, true, "Player");
    auto& trfP1 = registry_.get<TransformComponent>(player1);
    trfP1.SetPosition(-1, -1, 1);

    auto& renderp1 = registry_.get<RenderComponent>(player1);
    renderp1.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere));
    renderp1.color = { 0.f, 0.5f, 0.2f, 1.f };
    auto& channelp1 = registry_.get<InputChannelComponent>(player1);
    channelp1.channel = 0;
    channelp1.useWASD = false;
    registry_.emplace<WaveCollisionComponent>(player1);
    entt::entity colObj1 = registry_.get<HierarchyComponent>(player1).children[0];
    auto& colp1 = registry_.get<ColliderComponent>(colObj1);
    colp1.layer = CollisionLayer::Wave2;

    // Ground
    entt::entity ground = Prefab::CreateWorldObject(registry_, true, false, true, "Ground");
    auto& trfGnd = registry_.get<TransformComponent>(ground);
    trfGnd.SetPosition(0, -3, 0);
    trfGnd.SetScale(20, 0.1, 20);
    auto& renderGnd = registry_.get<RenderComponent>(ground);
    //renderGnd.color = { 0.639f, 0.8f, 0.639f, 1.f };
    renderGnd.color = { 0.710f, 0.839f, 0.573f, 1.f };

    engine_.collisionLayerMtx_.SetCollisionLayer(CollisionLayer::Wave1, CollisionLayer::Wave2, true);

    auto& wavePool = EntityPool<WavePoolTraits>::EmplaceInContext(registry_);
    auto& colliderPool = EntityPool<WaveColliderPoolTraits>::EmplaceInContext(registry_);
    registry_.ctx().emplace<WaveManager>(wavePool, colliderPool);

    EventDispatcher::GetInstance().Connect<TriggerEnterEvent, &WaveCollisionEnter>();
    EventDispatcher::GetInstance().Connect<TriggerExitEvent, &WaveCollisionExit>();
    EventDispatcher::GetInstance().Connect<LandingEvent, &MyState::MakeWaveJump>(*this);
}

void MyState::Update()
{
}

void MyState::Exit()
{}

void MyState::WaveCollisionEnter(const tomato::TriggerEnterEvent& event) {
    entt::entity root1 = GetRootEntity(event.reg, event.a);
    entt::entity root2 = GetRootEntity(event.reg, event.b);

    {
        auto* testComp = event.reg->try_get<WaveCollisionComponent>(root1);
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
    entt::entity root = GetRootEntity(event.reg, event.a);
    if (auto* testComp = event.reg->try_get<WaveCollisionComponent>(root))
    {
        if (auto* render = event.reg->try_get<RenderComponent>(root))
        {
            if (testComp->color.has_value())
                render->color = testComp->color.value();
        }
    }

    root = GetRootEntity(event.reg, event.b);
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
    if (event.remainingJumpCount == 0)
        registry_.ctx().get<WaveManager>().Acquire(registry_, event.e, event.position, 0.01f);
}
#include "TestState.h"
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
#include "CollisionTestComponent.h"
#include "Event/EventDispatcher.h"
#include "Prefab/Prefab.h"
#include "Prefab/UIPrefab.h"
#include "Serialization/ComponentSerializer.h"
#include "TimerTestComponent.h"
#include "ECS/Components/Nametag.h"
#include "Particle/ParticleEmitterPool.h"
#include "Utils/RegistryEntry.h"
REGISTER_STATE(TestState)

using namespace tomato;
using namespace std::chrono_literals;

void TestState::Init() {
    //// Load Assets
    auto audioTest = Audio::Create(PathManager::ProjectSound("sfx_get_heart.mp3"), 8);
    audioPtr_ = AssetRegistry<Audio>::GetInstance().Get(audioTest);

    Texture::Create(PathManager::ProjectImage("WATER_GAME_LOGO.png"));
    Texture::Create(PathManager::ProjectImage("heart.png"));
    Font::Create(PathManager::ProjectFont("D2Coding.ttf"));
    Font::Create(PathManager::ProjectFont("Pretendard-SemiBold.ttf"));
    Mesh::Create(Mesh::Primitive::Sphere, 20, 10);

    ParticleEffect::Create(PathManager::ProjectParticle("burst_test.tmt.ptc"));
    ParticleEffect::Create(PathManager::ProjectParticle("ribbon_particle.tmt.ptc"));
    ParticleEffect::Create(PathManager::ProjectParticle("jump.tmt.ptc"));

    EventDispatcher::GetInstance().Connect<LandingEvent, CallbackJump>();

    //// Set rollback
    engine_.SetRollbackComponent<MovementComponent>();
    engine_.SetRollbackComponent<VelocityComponent>();

    //// Create game object
    // Main camera
    entt::entity cam = Prefab::CreateCamera(registry_, "Camera", true);
    auto& trfCam = registry_.get<TransformComponent>(cam);
    trfCam.SetPosition(0, 8, 8);
    trfCam.SetRotationDegree(-50, 0, 0);

    PlayTest();
    //BottleneckTest();
}

void TestState::Update() {
    // if (engine_.GetInputRecorder().IsPress(InputIntent::Test_1))
    //     audioPtr_->Start();
}

void TestState::Exit() {}

void TestState::TEST_CollisionEnter(const tomato::CollisionEnterEvent& event) {
    entt::entity root = GetRootEntity(event.reg, event.e1);
    if (auto* testComp = event.reg->try_get<CollisionTestComponent>(root))
    {
        if (auto* render = event.reg->try_get<RenderComponent>(root))
        {
            if (!testComp->color.has_value())
                testComp->color = render->color;
            render->color = CollisionTestComponent::COLLISION_COLOR;
        }
    }

    root = GetRootEntity(event.reg, event.e2);
    if (auto* testComp = event.reg->try_get<CollisionTestComponent>(root))
    {
        if (auto* render = event.reg->try_get<RenderComponent>(root))
        {
            if (!testComp->color.has_value())
                testComp->color = render->color;
            render->color = CollisionTestComponent::COLLISION_COLOR;
        }
    }
}

void TestState::TEST_CollisionExit(const tomato::CollisionExitEvent& event) {
    entt::entity root = GetRootEntity(event.reg, event.e1);
    if (auto* testComp = event.reg->try_get<CollisionTestComponent>(root))
    {
        if (auto* render = event.reg->try_get<RenderComponent>(root))
            render->color = testComp->color.value();
    }

    root = GetRootEntity(event.reg, event.e2);
    if (auto* testComp = event.reg->try_get<CollisionTestComponent>(root))
    {
        if (auto* render = event.reg->try_get<RenderComponent>(root))
            render->color = testComp->color.value();
    }
}

void TestState::CallbackJump(const tomato::LandingEvent& event)
{
    auto e = event.reg->ctx().get<ParticleEmitterPool>().Acquire(
        GetAssetID(PathManager::ProjectParticle("jump.tmt.ptc")),
        event.position);
     std::cout << "Jump particle " << (int)e.value() << "\n";
}

void TestState::PlayTest()
{
    // Player0 character
    entt::entity player0 = Prefab::CreateCharacter(registry_, "Player 0", true);

    auto& trfP0 = registry_.get<TransformComponent>(player0);
    trfP0.SetPosition(1, 2, 0);

    auto& renderP0 = registry_.get<RenderComponent>(player0);
    renderP0.mesh = GetAssetID("Primitive::Sphere_20_10");
    renderP0.color = {155.f / 255, 20.f / 255, 90.f / 255, 0.8f};

    auto& channelP0 = registry_.get<InputChannelComponent>(player0);
    channelP0.channel = 0;

    registry_.emplace<CollisionTestComponent>(player0);

    auto& particlePool = registry_.ctx().get<ParticleEmitterPool>();
    particlePool.Acquire(GetAssetID("Resources\\Contents\\Particle\\burst_test.tmt.ptc"), GetUUID(registry_, player0));
    particlePool.Acquire(GetAssetID("Resources\\Contents\\Particle\\ribbon_particle.tmt.ptc"), GetUUID(registry_, player0));

    // Player1 character
    entt::entity player1 = Prefab::CreateCharacter(registry_, "Player 1", true);

    auto& trfP1 = registry_.get<TransformComponent>(player1);
    trfP1.SetPosition(-1, 2, 0);

    auto& renderP1 = registry_.get<RenderComponent>(player1);
    renderP1.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere));
    renderP1.color = { 8.f / 255, 75.f / 255, 109.f / 255, 0.8f };

    auto& channelP1 = registry_.get<InputChannelComponent>(player1);
    channelP1.channel = 1;

    // Ground
    entt::entity ground = Prefab::CreateWorldObject(registry_, "Ground", true);

    auto& trfGnd = registry_.get<TransformComponent>(ground);
    trfGnd.SetPosition(0, -0.05, 0);
    trfGnd.SetScale(10, 0.1, 10);

    auto& renderGnd = registry_.get<RenderComponent>(ground);
    renderGnd.color = { 129.f / 255, 172.f / 255, 167.f / 255, 1.f };

    //entt::entity objUp = Prefab::CreateStaticObject(registry_, Prefab::Primitive::Cube, { 0, 10, 0 });
    //entt::entity objUp = Prefab::CreateStaticObject(registry_, Prefab::Primitive::Cube, { 0.f, 12.f, 15.f });

    ////UI

    // auto btn = UIPrefab::CreateButton(registry_, { 0.f, -10.f });
    // auto& btnuiCmp = registry_.get<UIComponent>(btn);
    // btnuiCmp.sortOrder = 100;
    // auto& mouseEvt = registry_.get<MouseEventComponent>(btn);
    // mouseEvt.onClick =
    //     [this](const MouseClickEvent& e)
    //     {
    //         uiController_.onClick(e);
    //     };

    auto tBtn = UIPrefab::CreateButton(registry_, entt::null, { -10.f, -10.f });
    auto& tBtnUIComp = registry_.get<UIComponent>(tBtn);
    tBtnUIComp.sortOrder = 100;
    auto& tMouseEvt = registry_.get<MouseEventComponent>(tBtn);
#if 0
    auto& timerComp = registry_.emplace<TimerTestComponent>(tBtn);
    if (timerComp.timer.SetTimer(0h, 0min, 10s))
        TMT_INFO << "set timer";
    tMouseEvt.onClick = [&](const MouseClickEvent& e)
    {
        if (auto* timer = registry_.try_get<TimerTestComponent>(e.e))
        {
            switch (timer->timer.GetState())
            {
                case TimerState::Idle:
                    TMT_INFO << "   *** Timer start ***";
                    timer->timer.Start(e.tick);
                    break;
                case TimerState::Running:
                    timer->timer.Pause(e.tick);
                    break;
                case TimerState::Paused:
                    timer->timer.Start(e.tick);
                    break;
                case TimerState::Finished:
                    timer->timer.Reset();
                    break;
                default:
                    break;
            }
        }
    };
#elif 1

#endif

    UIPrefab::CreateText(registry_, entt::null, { 100.f, 0.f });
    UIPrefab::CreateImage(registry_, entt::null, PathManager::ProjectImage("WATER_GAME_LOGO.png"), {200.f, 300.f});
    //UIPrefab::CreateCanvas(registry_);
    //UIPrefab::CreateCanvas(registry_);
    //UIPrefab::CreateCanvas(registry_);
#if 0
    auto targetLabel = UIPrefab::CreateText(registry_, entt::null, { 0.f, 0.f }, "player0", { 1.0f, 1.0f, 0.f, 1.f }, 20.f);
    registry_.emplace<TargetComponent>(targetLabel, GetUUID(registry_, player0), glm::vec3{ 0.f, 1.f, 0.f });
    SetHierarchy(registry_, UIPrefab::GetCanvas(registry_), targetLabel);
    auto& uiCmp = registry_.get<UIComponent>(targetLabel);
    uiCmp.sortOrder = 1;

    auto targetLabel1 = UIPrefab::CreateText(registry_, entt::null, { 0.f, 0.f }, "player1", { 1.0f, 1.0f, 0.f, 1.f }, 20.f);
    registry_.emplace<TargetComponent>(targetLabel1, GetUUID(registry_, player1), glm::vec3{ 0.f, 1.f, 0.f });
    SetHierarchy(registry_, UIPrefab::GetCanvas(registry_), targetLabel1);
    auto& uiCmp1 = registry_.get<UIComponent>(targetLabel1);
    uiCmp1.sortOrder = 1;
#elif 1
    auto worldCanvas = UIPrefab::CreateCanvas(registry_, RenderMode::World);
    auto targetLabel = UIPrefab::CreateText(registry_, worldCanvas, { 0.f, 0.f }, "player0", { 1.0f, 1.0f, 0.f, 1.f }, 0.5f);
    registry_.emplace<TargetComponent>(targetLabel, GetUUID(registry_, player0), glm::vec3{ 0.f, 1.f, 0.f });
    SetHierarchy(registry_, worldCanvas, targetLabel);
    auto& uiCmp = registry_.get<UIComponent>(targetLabel);
    uiCmp.sortOrder = 1;

    auto targetLabel1 = UIPrefab::CreateText(registry_, worldCanvas, { 0.f, 0.f }, "player1", { 1.0f, 1.0f, 0.f, 1.f }, 0.5f);
    registry_.emplace<TargetComponent>(targetLabel1, GetUUID(registry_, player1), glm::vec3{ 0.f, 1.f, 0.f });
    SetHierarchy(registry_, worldCanvas, targetLabel1);
    auto& uiCmp1 = registry_.get<UIComponent>(targetLabel1);
    uiCmp1.sortOrder = 1;
#endif

    EventDispatcher::GetInstance().Connect<CollisionEnterEvent, &TEST_CollisionEnter>();
    EventDispatcher::GetInstance().Connect<CollisionExitEvent, &TEST_CollisionExit>();
}

void TestState::BottleneckTest()
{
    for (int x = 0; x < 1000; ++x)
    {
        entt::entity e = Prefab::CreateWorldObject(registry_, "GameObject", false);
        auto& trf = registry_.get<TransformComponent>(e);
        trf.SetPosition(2 * x, 0, 0);
    }
}

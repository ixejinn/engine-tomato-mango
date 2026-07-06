#include "TestState.h"
#include "Engine.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Audio/Audio.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "Input/InputRecorder.h"
#include "Input/InputConstants.h"
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

using namespace tomato;
using namespace std::chrono_literals;

void TestState::Init() {
    //// Audio test
    auto id = Audio::Create("Resources/Contents/sfx_get_heart.mp3", 8);
    audioPtr_ = AssetRegistry<Audio>::GetInstance().Get(id);
    Texture::Create("Resources/Contents/WATER_GAME_LOGO.png");
    Font::Create("Resources/Engine/Assets/Fonts/D2Coding.ttf");
    Font::Create("Resources/Engine/Assets/Fonts/Pretendard-SemiBold.ttf");

    engine_.GetInputRecorder().BindInputIntent(Key::J, InputIntent::Test_1);

    //// Set rollback
    engine_.SetRollbackComponent<MovementComponent>();
    engine_.SetRollbackComponent<VelocityComponent>();

    //// Create game object

    // Camera
    Prefab::CreateCamera(registry_,
        true,
        glm::vec3(0.f, 4.f, 10.f),
        // glm::vec3(0.f, 6.f, 0.f),
        glm::vec3(-30.f, 0.f, 0.f)
        // glm::vec3(-90.f, 0.f, 0.f),
        );

    //// Player0 character
    entt::entity player0 = Prefab::CreateCharacter(registry_, Prefab::Primitive::Cube, { 1, 2, 0 });
    auto& renderp0 = registry_.get<RenderComponent>(player0);
    renderp0.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere));
    renderp0.color = { 1.f, 1.f, 0.f, 1.f };
    auto& channelp0 = registry_.get<InputChannelComponent>(player0);
    channelp0.channel = 0;
    registry_.emplace<CollisionTestComponent>(player0);

    //// Player1 character
    entt::entity player1 = Prefab::CreateCharacter(registry_, Prefab::Primitive::Cube, { -1, 2, 0 });
    auto& renderp1 = registry_.get<RenderComponent>(player1);
    renderp1.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere));
    renderp1.color = { 0.f, 1.f, 1.f, 1.f };
    auto& channelp1 = registry_.get<InputChannelComponent>(player1);
    channelp1.channel = 1;

    // Ground
    entt::entity ground = Prefab::CreateStaticObject(registry_, Prefab::Primitive::Cube, { 0, -3, 0 });
    auto& trfGnd = registry_.get<TransformComponent>(ground);
    trfGnd.SetScale(10, 0.1, 10);
    auto& renderGnd = registry_.get<RenderComponent>(ground);
    renderGnd.color = { 0.f, 1.f, 0.f, 1.f };

    // Test billboarding
    entt::entity billboarding = registry_.create();
    registry_.emplace<NametagComponent>(billboarding, GenerateUUID(), GenerateEntityName(registry_, "billboarding"));
    registry_.emplace<TransformComponent>(billboarding, glm::vec3{3, 0, 0});
    registry_.emplace<RenderComponent>(billboarding,
        glm::vec4(1.f),
        GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Plain)),
        GetAssetID("ParticleShader"),
        GetAssetID(Texture::PrimitiveName));
    registry_.emplace<RootEntityTag>(billboarding);


    ////UI
    auto targetLabel = UIPrefab::CreateText(registry_, { 0.f, 0.f }, "player0", { 1.0f, 1.0f, 0.f, 1.f }, 20.f);
    registry_.emplace<TargetComponent>(targetLabel, GetUUID(registry_, player0));
    SetHierarchy(registry_, UIPrefab::GetCanvas(registry_), targetLabel);
    auto& uiCmp = registry_.get<UIComponent>(targetLabel);
    uiCmp.sortOrder = 1;

    auto targetLabel1 = UIPrefab::CreateText(registry_, { 0.f, 0.f }, "player1", {1.0f, 1.0f, 0.f, 1.f}, 20.f);
    registry_.emplace<TargetComponent>(targetLabel1, GetUUID(registry_, player1));
    SetHierarchy(registry_, UIPrefab::GetCanvas(registry_), targetLabel1);
    auto& uiCmp1 = registry_.get<UIComponent>(targetLabel1);
    uiCmp1.sortOrder = 1;

    // auto btn = UIPrefab::CreateButton(registry_, { 0.f, -10.f });
    // auto& btnuiCmp = registry_.get<UIComponent>(btn);
    // btnuiCmp.sortOrder = 100;
    // auto& mouseEvt = registry_.get<MouseEventComponent>(btn);
    // mouseEvt.onClick =
    //     [this](const MouseClickEvent& e)
    //     {
    //         uiController_.onClick(e);
    //     };

    auto tBtn = UIPrefab::CreateButton(registry_, { 0.f, -10.f });
    auto& tBtnUIComp = registry_.get<UIComponent>(tBtn);
    tBtnUIComp.sortOrder = 100;
    auto& tMouseEvt = registry_.get<MouseEventComponent>(tBtn);
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


    UIPrefab::CreateText(registry_, { 100.f, 0.f });
    UIPrefab::CreateImage(registry_, "Resources/Contents/WATER_GAME_LOGO.png", { 200.f, 300.f });
    //UIPrefab::CreateCanvas(registry_);
    //UIPrefab::CreateCanvas(registry_);
    //UIPrefab::CreateCanvas(registry_);
    //Serialization::SaveScene(registry_, "Resources/Engine/Assets/test.data");

    EventDispatcher::GetInstance().Connect<CollisionEnterEvent, &TEST_CollisionEnter>();
    EventDispatcher::GetInstance().Connect<CollisionExitEvent, &TEST_CollisionExit>();

    //std::cout << AssetRegistry<Audio>::GetInstance().GetName(id);
}

void TestState::Update() {
    if (engine_.GetInputRecorder().IsPress(InputIntent::Test_1))
        audioPtr_->Start();
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
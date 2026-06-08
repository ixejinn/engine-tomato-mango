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

#include "ECS/Components/Transform.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Text.h"
#include "ECS/Components/UI.h"
#include "ECS/Components/UIEvents.h"
#include "ECS/Components/Hierarchy.h"

#include "Collision/CollisionEvent.h"
#include "CollisionTestComponent.h"

#include "Prefab/Prefab.h"
#include "Prefab/UIPrefab.h"
using namespace tomato;

void TestState::Init() {
    // Audio test
    auto id = Audio::Create("Resources/Contents/sfx_get_heart.mp3", 8);
    audioPtr_ = AssetRegistry<Audio>::GetInstance().Get(id);

    Texture::Create("Resources/Contents/WATER_GAME_LOGO.png");

    engine_.GetInputRecorder().BindInputIntent(Key::J, InputIntent::Test_1);

    // Create game object

    // Camera
    const auto cam = registry_.create();
    registry_.emplace<TransformComponent>(cam,
                                          glm::vec3(0.f, 1.f, 10.f), glm::vec3(0.f, 0.f, 0.f));
                                           //glm::vec3(0.f, 5.f, 0.f), glm::vec3(-90.f, 0.f, 0.f));
                                           // glm::vec3(0.f, 7.5f, 15.f), glm::vec3(-30.f, 0.f, 0.f));
    auto& camComp = registry_.emplace<CameraComponent>(cam);
    camComp.mode = ProjectionMode::Perspective;
     // camComp.mode = ProjectionMode::Orthogonal;
    registry_.emplace<MainCameraTag>(cam);
    registry_.emplace<RootEntityTag>(cam);

    // Player character
    const auto me = registry_.create();
    auto& trfCompMe = registry_.emplace<TransformComponent>(me);

    registry_.emplace<VelocityComponent>(me);
    registry_.emplace<InputChannelComponent>(me, static_cast<uint8_t>(0), true);
    registry_.emplace<MovementComponent>(me);
    registry_.emplace<RenderComponent>(me,
                                       glm::vec4(1.f, 1.f, 0.f, 1.f),
                                        // GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube)),
                                       GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere)),
                                       GetAssetID(Shader::PrimitiveName),
                                       GetAssetID(Texture::PrimitiveName));
    registry_.emplace<RootEntityTag>(me);

    // Player collider
    const auto colP = registry_.create();
    SetHierarchy(registry_, me, colP);

    auto& trfColP = registry_.emplace<TransformComponent>(colP);
    // trfColP.SetPosition(0.f, 0.5f, 0.f);
    registry_.emplace<ColliderComponent>(colP, ColliderType::Cube);
    // registry_.emplace<ColliderComponent>(colP, ColliderType::Sphere);
    registry_.emplace<RenderComponent>(colP,
        glm::vec4(1.f, 1.f, 1.f, 1.f),
        GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube)),
        // GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere)),
        GetAssetID(Shader::PrimitiveName),
        GetAssetID(Texture::PrimitiveName));


    // You
    const auto you = registry_.create();
    auto& trfCompW = registry_.emplace<TransformComponent>(you, glm::vec3(-3, 0, 0), glm::vec3(0, 0, 0));

    registry_.emplace<VelocityComponent>(you);
    registry_.emplace<InputChannelComponent>(you, static_cast<uint8_t>(1), true);
    registry_.emplace<MovementComponent>(you);
    registry_.emplace<RenderComponent>(you,
        glm::vec4(1.f, 1.f, 1.f, 1.f),
        GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere)),
        GetAssetID(Shader::PrimitiveName),
        GetAssetID(Texture::PrimitiveName));
    registry_.emplace<RootEntityTag>(you);

    // collider
    const auto colW = registry_.create();
    SetHierarchy(registry_, you, colW);

    auto& trfColW = registry_.emplace<TransformComponent>(colW);
    registry_.emplace<ColliderComponent>(colW, ColliderType::Cube);
    registry_.emplace<RenderComponent>(colW,
        glm::vec4(1.f, 1.f, 1.f, 1.f),
        GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube)),
        GetAssetID(Shader::PrimitiveName),
        GetAssetID(Texture::PrimitiveName));

    //UI
#if 0
    const auto canvas = registry_.create();
    registry_.emplace<tomato::CanvasComponent>(canvas);
    registry_.emplace<tomato::UIComponent>(canvas, canvas);
    registry_.emplace<tomato::RectTransformComponent>(canvas);
    registry_.emplace<tomato::HierarchyComponent>(canvas);
    registry_.emplace<tomato::RenderComponent>(canvas,
        glm::vec4{ 1.f, 1.f, 1.f, 0.5f },
        GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::LBPlain)),
        GetAssetID("UIShader"),
        GetAssetID(Texture::PrimitiveName));


    const auto button = registry_.create();
    registry_.emplace<tomato::UIComponent>(button, canvas, 1);
    registry_.emplace<tomato::RectTransformComponent>(button, glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(200.f, 200.f), glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f));
    registry_.emplace<tomato::SelectableComponent>(button);
    registry_.emplace<tomato::MouseEventComponent>(button);
    registry_.emplace<tomato::HierarchyComponent>(button);
    SetHierarchy(registry_, canvas, button);
    registry_.emplace<tomato::RenderComponent>(button,
        glm::vec4{ 0.2f, 0.75f, 0.4f, 1.0f },
        GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::LBPlain)),
        GetAssetID("UIShader"),
        GetAssetID(Texture::PrimitiveName));

    const auto buttonText = registry_.create();
    registry_.emplace<tomato::UIComponent>(buttonText, canvas, 2);
    registry_.emplace<tomato::TextComponent>(buttonText, "Button1임", glm::vec4{ 0.3, 0.7f, 0.9f, 1.0f }, 30.f);
    registry_.emplace<tomato::RectTransformComponent>(buttonText, glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f));
    registry_.emplace<tomato::HierarchyComponent>(buttonText);
    SetHierarchy(registry_, button, buttonText);

    const auto TargetLabel = registry_.create();
    registry_.emplace<tomato::UIComponent>(TargetLabel, canvas, 2);
    registry_.emplace<tomato::TextComponent>(TargetLabel, "player1", glm::vec4{ 1.f, 1.f, 0.5f, 1.0f }, 24.f);
    registry_.emplace<tomato::RectTransformComponent>(TargetLabel, glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f));
    registry_.emplace<tomato::HierarchyComponent>(TargetLabel);
    registry_.emplace<tomato::TargetComponent>(TargetLabel, me);
    SetHierarchy(registry_, canvas, TargetLabel);
#elif 1
    //CreateCanvas(registry_);
    CreateButton(registry_);
    CreateText(registry_, { 100.f, 0.f });

    auto targetLabel = CreateText(registry_, { 0.f, 0.f }, "player1", {1.0f, 1.0f, 0.f, 1.f}, 20.f);
    registry_.emplace<TargetComponent>(targetLabel, me);
    SetHierarchy(registry_, GetCanvas(registry_), targetLabel);

    CreateImage(registry_, "Resources/Contents/WATER_GAME_LOGO.png", {200.f, 300.f});
#endif

}

void TestState::Update() {
    if (engine_.GetInputRecorder().IsPress(InputIntent::Test_1))
        audioPtr_->Start();
}

void TestState::Exit() {
}

void TestState::TEST_CollisionEnter(const tomato::CollisionEnterEvent& event, entt::entity e) {
    auto testComp = event.reg->try_get<CollisionTestComponent>(e);
    auto& render = event.reg->get<RenderComponent>(e);
    if (testComp) {
        testComp->color = render.color;
        render.color = CollisionTestComponent::COLLISION_COLOR;
    }
}

void TestState::TEST_CollisionExit(const tomato::CollisionExitEvent& event, entt::entity e) {
    auto testComp = event.reg->try_get<CollisionTestComponent>(e);
    auto& render = event.reg->get<RenderComponent>(e);
    if (testComp)
        render.color = testComp->color;
}

void TestState::TEST_TriggerEnter(const tomato::TriggerEnterEvent& event, entt::entity e) {
    auto testComp = event.reg->try_get<CollisionTestComponent>(e);
    auto& render = event.reg->get<RenderComponent>(e);
    if (testComp) {
        testComp->color = render.color;
        render.color = CollisionTestComponent::COLLISION_COLOR;
    }
}

void TestState::TEST_TriggerExit(const tomato::TriggerExitEvent& event, entt::entity e) {
    auto testComp = event.reg->try_get<CollisionTestComponent>(e);
    auto& render = event.reg->get<RenderComponent>(e);
    if (testComp)
        render.color = testComp->color;
}

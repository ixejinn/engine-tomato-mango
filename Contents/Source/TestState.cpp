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
using namespace tomato;

void TestState::Init() {
    // Audio test
    auto id = Audio::Create("Resources/Contents/sfx_get_heart.mp3", 8);
    audioPtr_ = AssetRegistry<Audio>::GetInstance().Get(id);

    engine_.GetInputRecorder().BindInputIntent(Key::J, InputIntent::Test_1);

    // Create game object

    // Camera
    Prefab::CreateCamera(registry_,
                         glm::vec3(0.f, 4.f, 10.f),
                         // glm::vec3(0.f, 6.f, 0.f),
                         glm::vec3(-30.f, 0.f, 0.f),
                         // glm::vec3(-90.f, 0.f, 0.f),
                         true);

    // Player1 character
    entt::entity player1 = Prefab::CreateCharacter(registry_, Prefab::Primitive::Sphere, {-1.5, 2, 0});
    // entt::entity center = Prefab::CreateCharacter(registry_, Prefab::Primitive::Cube, {0, 0, 0});
    auto& renderP1 = registry_.get<RenderComponent>(player1);
    renderP1.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere));
    renderP1.color = {1.f, 1.f, 0.f, 1.f};

     // NPC west
     // entt::entity west = Prefab::CreateStaticObject(registry_, Prefab::Primitive::Cube, {0, -2.45, 0});
     // // entt::entity west = Prefab::CreateStaticObject(registry_, Prefab::Primitive::Cube, {-2, 0, 0});
     // auto& renderW = registry_.get<RenderComponent>(west);
     // renderW.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere));
     // renderW.color = {1.f, 0.f, 0.f, 1.f};

    // Player2 character
    entt::entity player2 = Prefab::CreateCharacter(registry_, Prefab::Primitive::Sphere, {1.5, 2, 0});
    auto& renderP2 = registry_.get<RenderComponent>(player2);
    renderP2.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere));
    renderP2.color = {0.f, 1.f, 1.f, 1.f};
    auto& inputP2 = registry_.get<InputChannelComponent>(player2);
    inputP2.is1P = false;

    // Ground
    entt::entity ground = Prefab::CreateStaticObject(registry_, Prefab::Primitive::Cube, {0, -3, 0});
    auto& trfGnd = registry_.get<TransformComponent>(ground);
    trfGnd.SetScale(10, 0.1, 10);
    auto& renderGnd = registry_.get<RenderComponent>(ground);
    renderGnd.color = {0.f, 1.f, 0.f, 1.f};


    //UI
    const auto canvas = registry_.create();
    registry_.emplace<tomato::CanvasComponent>(canvas);
    registry_.emplace<tomato::UIComponent>(canvas, canvas);
    registry_.emplace<tomato::RectTransformComponent>(canvas);
    registry_.emplace<tomato::HierarchyComponent>(canvas);
    registry_.emplace<tomato::RenderComponent>(canvas,
        glm::vec4{ 1.f, 1.f, 1.f, 0.f },
        GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::LBPlain)),
        GetAssetID("UIShader"),
        GetAssetID(Texture::PrimitiveName));
    //
    //
    // const auto button = registry_.create();
    // registry_.emplace<tomato::UIComponent>(button, canvas, 1);
    // registry_.emplace<tomato::RectTransformComponent>(button, glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(200.f, 200.f), glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f));
    // registry_.emplace<tomato::SelectableComponent>(button);
    // registry_.emplace<tomato::MouseEventComponent>(button);
    // registry_.emplace<tomato::HierarchyComponent>(button);
    // SetHierarchy(registry_, canvas, button);
    // registry_.emplace<tomato::RenderComponent>(button,
    //     glm::vec4{ 0.2f, 0.75f, 0.4f, 1.0f },
    //     GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::LBPlain)),
    //     GetAssetID("UIShader"),
    //     GetAssetID(Texture::PrimitiveName));
    //
    // const auto buttonText = registry_.create();
    // registry_.emplace<tomato::UIComponent>(buttonText, canvas, 2);
    // registry_.emplace<tomato::TextComponent>(buttonText, "Button1임", glm::vec4{ 0.3, 0.7f, 0.9f, 1.0f }, 30.f);
    // registry_.emplace<tomato::RectTransformComponent>(buttonText, glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f));
    // registry_.emplace<tomato::HierarchyComponent>(buttonText);
    // SetHierarchy(registry_, button, buttonText);

    const auto labelP1 = registry_.create();
    registry_.emplace<tomato::UIComponent>(labelP1, canvas, 2);
    registry_.emplace<tomato::TextComponent>(labelP1, "player1", glm::vec4{ 1.f, 1.f, 0.5f, 1.0f }, 24.f);
    registry_.emplace<tomato::RectTransformComponent>(labelP1, glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f));
    registry_.emplace<tomato::HierarchyComponent>(labelP1);
    registry_.emplace<tomato::TargetComponent>(labelP1, player1);
    SetHierarchy(registry_, canvas, labelP1);

    const auto labelP2 = registry_.create();
    registry_.emplace<tomato::UIComponent>(labelP2, canvas, 2);
    registry_.emplace<tomato::TextComponent>(labelP2, "player2", glm::vec4{ 0.5f, 1.f, 1.f, 1.0f }, 24.f);
    registry_.emplace<tomato::RectTransformComponent>(labelP2, glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f));
    registry_.emplace<tomato::HierarchyComponent>(labelP2);
    registry_.emplace<tomato::TargetComponent>(labelP2, player2);
    SetHierarchy(registry_, canvas, labelP2);
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

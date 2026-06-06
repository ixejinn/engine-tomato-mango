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
                         glm::vec3(0.f, 7.5f, 15.f),
                         glm::vec3(-30.f, 0.f, 0.f),
                         true);

    // Player character
    entt::entity center = Prefab::CreateCharacter(registry_, Prefab::Primitive::Cube);
    auto& renderCe = registry_.get<RenderComponent>(center);
    renderCe.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere));
    renderCe.color = {1.f, 1.f, 0.f, 1.f};
//    const auto me = registry_.create();
//    auto& trfCompMe = registry_.emplace<TransformComponent>(me);
//    // trfCompMe.SetScale(2.f, 1.f, 1.f);
//    // trfCompMe.SetScale(2.f, 2.f, 1.f);
//    // trfCompMe.SetEulerDegree(0.f, 45.f, 0.f);
//    registry_.emplace<VelocityComponent>(me);
//    registry_.emplace<InputChannelComponent>(me, static_cast<uint8_t>(0), true);
//    registry_.emplace<MovementComponent>(me);
//     // registry_.emplace<ColliderComponent>(me, ColliderType::Cube);
////    registry_.emplace<ColliderComponent>(me, ColliderType::Sphere, trfCompMe);
//    // registry_.emplace<ColliderComponent>(me, ColliderType::Cube, trfCompMe, true);
//
//    registry_.emplace<RenderComponent>(me,
//                                       glm::vec4(1.f, 1.f, 0.f, 1.f),
//                                        // GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube)),
//                                       GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere)),
//                                       GetAssetID(Shader::PrimitiveName),
//                                       GetAssetID(Texture::PrimitiveName));
//    registry_.emplace<RootEntityTag>(me);
//
//    // Player collider
//    const auto colP = registry_.create();
//    SetHierarchy(registry_, me, colP);
//
//    auto& trfColP = registry_.emplace<TransformComponent>(colP);
//    // trfColP.SetPosition(0.f, 0.5f, 0.f);
//    registry_.emplace<ColliderComponent>(colP, ColliderType::Cube);
//    // registry_.emplace<ColliderComponent>(colP, ColliderType::Sphere);
//    registry_.emplace<RenderComponent>(colP,
//        glm::vec4(1.f, 1.f, 1.f, 1.f),
//        GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube)),
//        // GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere)),
//        GetAssetID(Shader::PrimitiveName),
//        GetAssetID(Texture::PrimitiveName));

     // NPC west
     entt::entity west = Prefab::CreateStaticObject(registry_, Prefab::Primitive::Cube, {-3, 0, 0});
     auto& renderW = registry_.get<RenderComponent>(west);
     renderW.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere));
     renderW.color = {1.f, 1.f, 1.f, 1.f};
//     const auto west = registry_.create();
//
//     auto& trfCompW = registry_.emplace<TransformComponent>(west,
//                                                            glm::vec3(-3, 0, 0), glm::vec3(0, 0, 0));
////     registry_.emplace<ColliderComponent>(west, ColliderType::Cube);
//     registry_.emplace<RenderComponent>(west,
//                                        glm::vec4(1.f, 1.f, 1.f, 1.f),
//                                        // GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube)),
//                                        GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere)),
//                                        GetAssetID(Shader::PrimitiveName),
//                                        GetAssetID(Texture::PrimitiveName));
//
//    // NPC west collider
//    const auto colW = registry_.create();
//    SetHierarchy(registry_, west, colW);
//
//    auto& trfColW = registry_.emplace<TransformComponent>(colW);
//    registry_.emplace<ColliderComponent>(colW, ColliderType::Cube);
//    // registry_.emplace<ColliderComponent>(colW, ColliderType::Sphere);
//    registry_.emplace<RenderComponent>(colW,
//        glm::vec4(1.f, 1.f, 1.f, 1.f),
//        GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube)),
//        // GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere)),
//        GetAssetID(Shader::PrimitiveName),
//        GetAssetID(Texture::PrimitiveName));


    //UI
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
    registry_.emplace<tomato::TargetComponent>(TargetLabel, center);
    SetHierarchy(registry_, canvas, TargetLabel);

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

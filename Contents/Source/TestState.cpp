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

#include "Collision/CollisionEvent.h"
#include "CollisionTestComponent.h"
using namespace tomato;

void TestState::Init() {
    // Audio test
    auto id = Audio::Create("Resources/Contents/sfx_get_heart.mp3", 8);
    audioPtr_ = AssetRegistry<Audio>::GetInstance().Get(id);

    engine_.GetInputRecorder().BindInputIntent(Key::J, InputIntent::Test_1);

    // Create game object

    // Camera
    const auto cam = registry_.create();
    registry_.emplace<TransformComponent>(cam,
//                                          glm::vec3(0.f, 1.f, 10.f), glm::vec3(0.f, 0.f, 0.f));
                                           glm::vec3(0.f, 5.f, 0.f), glm::vec3(-90.f, 0.f, 0.f));
//                                           glm::vec3(0.f, 7.5f, 15.f), glm::vec3(-30.f, 0.f, 0.f));
    auto& camComp = registry_.emplace<CameraComponent>(cam);
//    camComp.mode = ProjectionMode::Perspective;
     camComp.mode = ProjectionMode::Orthogonal;
    registry_.emplace<MainCameraTag>(cam);

    // Player character
    const auto me = registry_.create();
    auto& trfCompMe = registry_.emplace<TransformComponent>(me);
    // trfCompMe.SetScale(2.f, 1.f, 1.f);
    // trfCompMe.SetScale(2.f, 2.f, 1.f);
    // trfCompMe.SetEulerDegree(0.f, 45.f, 0.f);
    registry_.emplace<VelocityComponent>(me);
    registry_.emplace<InputChannelComponent>(me, static_cast<uint8_t>(0), true);
    registry_.emplace<MovementComponent>(me);
//     registry_.emplace<ColliderComponent>(me, ColliderType::Cube, trfCompMe);
//    registry_.emplace<ColliderComponent>(me, ColliderType::Sphere, trfCompMe);
    registry_.emplace<ColliderComponent>(me, ColliderType::Cube, trfCompMe, true);

    registry_.emplace<RenderComponent>(me,
                                       glm::vec4(1.f, 1.f, 0.f, 1.f),
                                        GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube)),
//                                       GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere)),
                                       GetAssetID(Shader::PrimitiveName),
                                       GetAssetID(Texture::PrimitiveName));

    registry_.emplace<CollisionTestComponent>(me);
    auto& onColCompMe = registry_.emplace<OnCollisionComponent>(me);
    onColCompMe.enter = TEST_CollisionEnter;
    onColCompMe.exit = TEST_CollisionExit;

    auto& onTgrCompMe = registry_.emplace<OnTriggerComponent>(me);
    onTgrCompMe.enter = TEST_TriggerEnter;
    onTgrCompMe.exit = TEST_TriggerExit;

    // NPC east
    const auto east = registry_.create();
    auto& trfCompE = registry_.emplace<TransformComponent>(east,
                                          glm::vec3(3, 0, 0), glm::vec3(0, 0, 0));
    registry_.emplace<VelocityComponent>(east);
    registry_.emplace<InputChannelComponent>(east, static_cast<uint8_t>(0), false);
    registry_.emplace<MovementComponent>(east);
     registry_.emplace<ColliderComponent>(east, ColliderType::Cube, trfCompE);
//    registry_.emplace<ColliderComponent>(east, ColliderType::Sphere, trfCompE);
    registry_.emplace<RenderComponent>(east,
                                       glm::vec4(0.f, 0.f, 1.f, 1.f),
                                        GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube)),
//                                       GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere)),
                                       GetAssetID(Shader::PrimitiveName),
                                       GetAssetID(Texture::PrimitiveName));

    registry_.emplace<CollisionTestComponent>(east);
    auto& onColCompE = registry_.emplace<OnCollisionComponent>(east);
    onColCompE.enter = TEST_CollisionEnter;
    onColCompE.exit = TEST_CollisionExit;

    // NPC west
    const auto west = registry_.create();
    auto& trfCompW = registry_.emplace<TransformComponent>(west,
                                                           glm::vec3(-3, 0, 0), glm::vec3(0, 0, 0));
    registry_.emplace<ColliderComponent>(west, ColliderType::Cube, trfCompW);
    registry_.emplace<RenderComponent>(west,
                                       glm::vec4(1.f, 1.f, 1.f, 1.f),
                                       GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube)),
                                       GetAssetID(Shader::PrimitiveName),
                                       GetAssetID(Texture::PrimitiveName));

    registry_.emplace<CollisionTestComponent>(west);
    auto& onColCompW = registry_.emplace<OnCollisionComponent>(west);
    onColCompW.enter = TEST_CollisionEnter;
    onColCompW.exit = TEST_CollisionExit;
}

void TestState::Update() {
    if (engine_.GetInputRecorder().IsPress(InputIntent::Test_1))
        audioPtr_->Start();
}

void TestState::Exit() {
}

void TestState::TEST_CollisionEnter(const tomato::CollisionEnterEvent& event, entt::entity e) {
    auto& trfScl = event.reg->get<TransformComponent>(e).GetScale();
    auto& colScl = event.reg->get<ColliderComponent>(e).halfExtents;
    // TMT_INFO << static_cast<uint32_t>(e);
    // TMT_INFO << "trf: " << trfScl.x << ", " << trfScl.y << ", " << trfScl.z;
    // TMT_INFO << "col: " << colScl.x << ", " << colScl.y << ", " << colScl.z;

    auto testComp = event.reg->try_get<CollisionTestComponent>(e);
    auto& render = event.reg->get<RenderComponent>(e);
    if (testComp) {
        testComp->color = render.color;
        render.color = CollisionTestComponent::COLLISION_COLOR;
    }
}

void TestState::TEST_CollisionExit(const tomato::CollisionExitEvent& event, entt::entity e) {
    auto& trfScl = event.reg->get<TransformComponent>(e).GetScale();
    auto& colScl = event.reg->get<ColliderComponent>(e).halfExtents;
    // TMT_INFO << static_cast<uint32_t>(e);
    // TMT_INFO << "trf: " << trfScl.x << ", " << trfScl.y << ", " << trfScl.z;
    // TMT_INFO << "col: " << colScl.x << ", " << colScl.y << ", " << colScl.z;

    auto testComp = event.reg->try_get<CollisionTestComponent>(e);
    auto& render = event.reg->get<RenderComponent>(e);
    if (testComp)
        render.color = testComp->color;
}

void TestState::TEST_TriggerEnter(const tomato::TriggerEnterEvent& event, entt::entity e) {
    auto& trfScl = event.reg->get<TransformComponent>(e).GetScale();
    auto& colScl = event.reg->get<ColliderComponent>(e).halfExtents;
    // TMT_INFO << static_cast<uint32_t>(e);
    // TMT_INFO << "trf: " << trfScl.x << ", " << trfScl.y << ", " << trfScl.z;
    // TMT_INFO << "col: " << colScl.x << ", " << colScl.y << ", " << colScl.z;

    auto testComp = event.reg->try_get<CollisionTestComponent>(e);
    auto& render = event.reg->get<RenderComponent>(e);
    if (testComp) {
        testComp->color = render.color;
        render.color = CollisionTestComponent::COLLISION_COLOR;
    }
}

void TestState::TEST_TriggerExit(const tomato::TriggerExitEvent& event, entt::entity e) {
    auto& trfScl = event.reg->get<TransformComponent>(e).GetScale();
    auto& colScl = event.reg->get<ColliderComponent>(e).halfExtents;
    // TMT_INFO << static_cast<uint32_t>(e);
    // TMT_INFO << "trf: " << trfScl.x << ", " << trfScl.y << ", " << trfScl.z;
    // TMT_INFO << "col: " << colScl.x << ", " << colScl.y << ", " << colScl.z;

    auto testComp = event.reg->try_get<CollisionTestComponent>(e);
    auto& render = event.reg->get<RenderComponent>(e);
    if (testComp)
        render.color = testComp->color;
}
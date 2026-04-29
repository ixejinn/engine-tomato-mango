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
                                          glm::vec3(0.f, 7.5f, 15.f), glm::vec3(-30.f, 0.f, 0.f));
    auto& camera = registry_.emplace<CameraComponent>(cam);
    camera.mode = ProjectionMode::Perspective;
//    camera.mode = ProjectionMode::Orthogonal;
    registry_.emplace<MainCameraTag>(cam);

    // Player character
    const auto me = registry_.create();
    auto& trf = registry_.emplace<TransformComponent>(me);
    registry_.emplace<SpeedComponent>(me, 2.f);
    registry_.emplace<InputChannelComponent>(me, static_cast<uint8_t>(0));
    registry_.emplace<JumpComponent>(me);
    registry_.emplace<ColliderComponent>(me, ColliderType::Cube);
    registry_.emplace<RenderComponent>(me,
                                       glm::vec4(1.f, 1.f, 0.f, 1.f),
                                       GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube)),
                                       GetAssetID(Shader::PrimitiveName),
                                       GetAssetID(Texture::PrimitiveName));

    registry_.emplace<CollisionTestComponent>(me);
    auto& colTest = registry_.emplace<OnCollisionComponent>(me);
    colTest.enter = TestState::TEST_CollisionEnter;
    colTest.exit = TestState::TEST_CollisionExit;

    // NPCs
    const auto east = registry_.create();
    registry_.emplace<TransformComponent>(east,
                                          glm::vec3(5, 0, 0), glm::vec3(90, 0, 0));
    registry_.emplace<SpeedComponent>(east, 2.f);
    registry_.emplace<InputChannelComponent>(east, static_cast<uint8_t>(1));
    registry_.emplace<JumpComponent>(east);
    registry_.emplace<ColliderComponent>(east, ColliderType::Cube);
    registry_.emplace<RenderComponent>(east,
                                       glm::vec4(0.f, 0.f, 1.f, 1.f),
                                       GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube)),
                                       GetAssetID(Shader::PrimitiveName),
                                       GetAssetID(Texture::PrimitiveName));
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
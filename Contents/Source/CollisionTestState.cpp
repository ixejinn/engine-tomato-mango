#include "CollisionTestState.h"
#include "Prefab/Prefab.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Transform.h"
#include "Resource/AssetHash.h"
#include "Resource/Render/Mesh.h"

using namespace tomato;

void CollisionTestState::Init()
{
    // Camera
    Prefab::CreateCamera(registry_,
        true,
        glm::vec3(0.f, 4.f, 10.f),
        // glm::vec3(0.f, 6.f, 0.f),
        glm::vec3(-30.f, 0.f, 0.f)
        // glm::vec3(-90.f, 0.f, 0.f),
        );

    //// Player0 character
    entt::entity player0 = Prefab::CreateCharacter(registry_, Prefab::Primitive::Cube, { 1, 1, 0 });
    // entt::entity player0 = Prefab::CreateCharacter(registry_, Prefab::Primitive::Cube, { 1, 0.510121, 0 });
    auto& renderp0 = registry_.get<RenderComponent>(player0);
    renderp0.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere));
    renderp0.color = { 1.f, 1.f, 0.f, 1.f };
    auto& channelp0 = registry_.get<InputChannelComponent>(player0);
    channelp0.channel = 0;

    // Ground
    entt::entity ground = Prefab::CreateStaticObject(registry_, Prefab::Primitive::Cube, { 0, -0.05, 0 });
    auto& trfGnd = registry_.get<TransformComponent>(ground);
    trfGnd.SetScale(10, 0.1, 10);
    auto& renderGnd = registry_.get<RenderComponent>(ground);
    renderGnd.color = { 0.f, 1.f, 0.f, 1.f };
}

void CollisionTestState::Update()
{

}

void CollisionTestState::Exit()
{

}
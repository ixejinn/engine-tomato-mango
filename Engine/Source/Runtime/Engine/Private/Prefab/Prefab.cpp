#include <entt/entt.hpp>
#include "Prefab/Prefab.h"
#include "Prefab/Character/CharacterMovement.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Camera.h"
#include "ECS/Entity/Hierarchy.h"
#include "Resource/AssetHash.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "Utils/Logger.h"

namespace tomato {
    entt::entity Prefab::CreateStaticObject(entt::registry& reg,
                                            Primitive mesh, glm::vec3 pos) {
        const entt::entity obj = reg.create();

        reg.emplace<TransformComponent>(obj, pos);
        reg.emplace<RenderComponent>(obj,
                                     glm::vec4(1.f),
                                     GetAssetID(Mesh::GetPrimitiveName(
                                             mesh == Cube ? Mesh::Primitive::Cube : Mesh::Primitive::Sphere)),
                                     GetAssetID(Shader::PrimitiveName),
                                     GetAssetID(Texture::PrimitiveName));

        const entt::entity col = AttachCollider(reg, obj, mesh == Cube ? ColliderType::Cube : ColliderType::Sphere);

        TMT_INFO << "Create static object: " << (int)obj;
        return obj;
    }

    entt::entity Prefab::CreateCharacter(entt::registry& reg,
                                         Primitive mesh, glm::vec3 pos) {
        const entt::entity obj = reg.create();

        reg.emplace<TransformComponent>(obj, pos);
        reg.emplace<RenderComponent>(obj,
                                     glm::vec4(1.f),
                                     GetAssetID(Mesh::GetPrimitiveName(
                                             mesh == Cube ? Mesh::Primitive::Cube : Mesh::Primitive::Sphere)),
                                     GetAssetID(Shader::PrimitiveName),
                                     GetAssetID(Texture::PrimitiveName));

        const entt::entity col = AttachCharacterCollider(reg, obj, mesh == Cube ? ColliderType::Cube : ColliderType::Sphere);

        reg.emplace<VelocityComponent>(obj);
        reg.emplace<InputChannelComponent>(obj);    //////////////////// 수정
        reg.emplace<MovementComponent>(obj);

        TMT_INFO << "Create character: " << (int)obj;
        return obj;
    }

    entt::entity Prefab::CreateCamera(entt::registry& reg,
                                      glm::vec3 pos, glm::vec3 rot,
                                      bool isMain) {
        const entt::entity obj = reg.create();

        reg.emplace<TransformComponent>(obj, pos, rot);
        reg.emplace<CameraComponent>(obj);
        reg.emplace<RootEntityTag>(obj);

        if (isMain)
            reg.emplace<MainCameraTag>(obj);

        TMT_INFO << "Create camera: " << (int)obj;
        return obj;
    }

    entt::entity Prefab::AttachCollider(entt::registry& reg, entt::entity parent, ColliderType type) {
        const entt::entity col = reg.create();
        SetHierarchy(reg, parent, col);

        reg.emplace<TransformComponent>(col);
        reg.emplace<ColliderComponent>(col, type);
        reg.emplace<RenderComponent>(col,
                                     glm::vec4(1.f),
                                     GetAssetID(Mesh::GetPrimitiveName(
                                             type == ColliderType::Cube ? Mesh::Primitive::Cube : Mesh::Primitive::Sphere)),
                                     GetAssetID(Shader::PrimitiveName),
                                     GetAssetID(Texture::PrimitiveName));

        TMT_INFO << "Create collider: " << (int)col;
        return col;
    }

    entt::entity Prefab::AttachCharacterCollider(entt::registry& reg, entt::entity parent, ColliderType type) {
        const entt::entity col = AttachCollider(reg, parent, type);
        const entt::entity ground = AttachCollider(reg, col, type);

        TransformComponent& trfP = reg.get<TransformComponent>(parent);
        TransformComponent& trfC = reg.get<TransformComponent>(ground);

        trfC.SetScale(trfP.GetLocalScale() * 0.8f);
        trfC.AddPosition({0.f, -(trfP.GetLocalScale().y * 0.1 + COLLISION_SKIN + 0.001f), 0.f});

        reg.get<ColliderComponent>(ground).isTrigger = true;

        auto& trg = reg.emplace<OnTriggerComponent>(ground);
        trg.enter = CharacterMovement::AfterLanding;
        trg.exit = CharacterMovement::StartFalling;

        TMT_INFO << "Create character collider, ground: " << (int)col << ", " << (int)ground;
        return col;
    }
}
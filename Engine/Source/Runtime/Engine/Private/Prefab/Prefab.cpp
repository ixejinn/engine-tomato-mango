#include <entt/entt.hpp>
#include "Prefab/Prefab.h"
#include "GameObject/Character/CharacterMovement.h"
#include "ECS/Components/CharComponents.h"
#include "ECS/Components/PhysComponents.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Rollback.h"
#include "ECS/Components/Nametag.h"
#include "ECS/Components/Visibility.h"
#include "ECS/Components/Character.h"
#include "ECS/Entity/Hierarchy.h"
#include "Resource/AssetHash.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "ECS/Entity/Entity.h"
#include "Utils/Logger.h"

namespace tomato::Prefab
{
    entt::entity CreateEmpty(entt::registry& reg, Primitive mesh, glm::vec3 pos)
    {
        const entt::entity obj = reg.create();

        reg.emplace<NametagComponent>(obj, GenerateUUID(), GenerateEntityName(reg, "GameObject"));
        reg.emplace<VisibilityComponent>(obj);
        reg.emplace<TransformComponent>(obj, pos);
        reg.emplace<RootEntityTag>(obj);

        TMT_INFO << "Create Empty object: " << (int)obj;
        return obj;
    }

    entt::entity CreateStaticObject(entt::registry& reg,
                                            Primitive mesh, glm::vec3 pos) {
        const entt::entity obj = reg.create();

        reg.emplace<NametagComponent>(obj, GenerateUUID(), GenerateEntityName(reg, "Object"));
        reg.emplace<TransformComponent>(obj, pos);
        reg.emplace<RenderComponent>(obj,
                                     glm::vec4(1.f),
                                     GetAssetID(Mesh::GetPrimitiveName(
                                             mesh == Cube ? Mesh::Primitive::Cube : Mesh::Primitive::Sphere)),
                                     GetAssetID(Shader::PrimitiveName),
                                     GetAssetID(Texture::PrimitiveName));
        reg.emplace<VisibilityComponent>(obj);
        const entt::entity col = AttachCollider(reg, obj, mesh == Cube ? ColliderType::Cube : ColliderType::Sphere);

        TMT_INFO << "Create static object: " << (int)obj;
        return obj;
    }

    entt::entity CreateCharacter(entt::registry& reg,
                                         Primitive mesh, glm::vec3 pos) {
        const entt::entity obj = reg.create();

        reg.emplace<NametagComponent>(obj, GenerateUUID(), GenerateEntityName(reg, "Character"));
        reg.emplace<TransformComponent>(obj, pos);
        reg.emplace<RenderComponent>(obj,
                                     glm::vec4(1.f),
                                     GetAssetID(Mesh::GetPrimitiveName(
                                             mesh == Cube ? Mesh::Primitive::Cube : Mesh::Primitive::Sphere)),
                                     GetAssetID(Shader::PrimitiveName),
                                     GetAssetID(Texture::PrimitiveName));
        reg.emplace<VisibilityComponent>(obj);

        const entt::entity col = AttachCharacterCollider(reg, obj, mesh == Cube ? ColliderType::Cube : ColliderType::Sphere);

        reg.emplace<VelocityComponent>(obj);
        reg.emplace<InputChannelComponent>(obj);    //////////////////// 수정
        reg.emplace<MovementComponent>(obj);
        reg.emplace<CharacterTag>(obj);
        reg.emplace<RollbackEntityTag>(obj);

        TMT_INFO << "Create character: " << (int)obj;
        return obj;
    }

    entt::entity CreateCamera(entt::registry& reg, bool isMain,
                                      glm::vec3 pos, glm::vec3 rot
                                      ) {
        const entt::entity obj = reg.create();

        reg.emplace<NametagComponent>(obj, GenerateUUID(), GenerateEntityName(reg, "Camera"));
        reg.emplace<TransformComponent>(obj, pos, rot);
        reg.emplace<CameraComponent>(obj);
        reg.emplace<RootEntityTag>(obj);
        reg.emplace<VisibilityComponent>(obj);
        // reg.emplace<RollbackEntityTag>(obj);

        if (isMain)
            reg.emplace<MainCameraTag>(obj);

        TMT_INFO << "Create camera: " << (int)obj;
        return obj;
    }

    entt::entity AttachCollider(entt::registry& reg, entt::entity parent, ColliderType type) {
        const entt::entity col = reg.create();
        reg.emplace<NametagComponent>(col, GenerateUUID(), GenerateEntityName(reg, "Collider"));

        SetHierarchy(reg, parent, col);

        reg.emplace<TransformComponent>(col);
        reg.emplace<ColliderComponent>(col, type);
        reg.emplace<RenderComponent>(col,
                                     glm::vec4(1.f),
                                     GetAssetID(Mesh::GetPrimitiveName(
                                             type == ColliderType::Cube ? Mesh::Primitive::Cube : Mesh::Primitive::Sphere)),
                                     GetAssetID(Shader::PrimitiveName),
                                     GetAssetID(Texture::PrimitiveName));
        reg.emplace<VisibilityComponent>(col);

        TMT_INFO << "Create collider: " << (int)col;
        return col;
    }

    entt::entity AttachCharacterCollider(entt::registry& reg, entt::entity parent, ColliderType type) {
        const entt::entity col = AttachCollider(reg, parent, type);
        const entt::entity ground = AttachCollider(reg, col, type);

        TransformComponent& trfP = reg.get<TransformComponent>(parent);
        TransformComponent& trfC = reg.get<TransformComponent>(ground);

        trfC.SetScale(trfP.GetLocalScale() * 0.8f);
        trfC.AddPosition({0.f, -(trfP.GetLocalScale().y * 0.1 + COLLISION_SKIN + 0.001f), 0.f});

        reg.get<ColliderComponent>(ground).isTrigger = true;

        TMT_INFO << "Create character collider, ground: " << (int)col << ", " << (int)ground;
        return col;
    }
}
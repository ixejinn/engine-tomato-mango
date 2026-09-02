#include <entt/entt.hpp>
#include "Prefab/Prefab.h"
#include "GameObject/Character/CharacterConfig.h"
#include "ECS/Components/Components.h"
#include "ECS/Entity/Hierarchy.h"
#include "Resource/AssetHash.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "ECS/Entity/Entity.h"
#include "Utils/Logger.h"

namespace tomato::Prefab
{
    entt::entity CreateBaseEntity(
        entt::registry& registry, const std::string& name, const bool root)
    {
        const entt::entity obj = registry.create();

        registry.emplace<NametagComponent>(obj, GenerateUUID(), registry.ctx().get<EntityNameGenerator>().Generate(name));
        registry.emplace<VisibilityComponent>(obj);
        //registry.emplace<LifetimeComponent>(obj);
        registry.emplace<TransformComponent>(obj);
        if (root)
            registry.emplace<RootEntityTag>(obj);

        return obj;
    }

    entt::entity CreateCamera(
        entt::registry& registry, const std::string& name,
        const bool main, const glm::vec3& pos, const glm::vec3& rot)
    {
        const entt::entity obj = CreateBaseEntity(registry, name);

        registry.emplace<CameraComponent>(obj);

        if (main)
            registry.emplace<MainCameraTag>(obj);

        auto& trf = registry.get<TransformComponent>(obj);
        trf.SetPosition(pos);
        trf.SetRotationDegree(rot);

        return obj;
    }

    entt::entity CreateStaticMesh(entt::registry& registry, const std::string& name)
    {
        const entt::entity obj = CreateBaseEntity(registry, name);

        registry.emplace<RenderComponent>(obj);

        return obj;
    }

    entt::entity CreateTriggerVolume(entt::registry& registry, const std::string& name)
    {
        const entt::entity obj = CreateBaseEntity(registry, name);

        registry.emplace<ColliderComponent>(obj, true);

        return obj;
    }

    entt::entity CreateWorldObject(entt::registry& registry, const std::string& name, bool printInfo)
    {
        const entt::entity obj = CreateBaseEntity(registry, name);

        registry.emplace<RenderComponent>(obj);
        const entt::entity col = AttachColliderEntity(registry, obj, false);

        if (printInfo)
        {
            TMT_INFO << "[WorldObject] " << std::left << std::setw(12) << name << "\n"
                     << "              entity   ID: " << std::right << std::setw(4) << (int)obj
                     << "              collider ID: " << std::right << std::setw(4) << (int)col;
        }
        return obj;
    }

    entt::entity CreateCharacter(entt::registry& registry, const std::string& name, bool printInfo)
    {
        const entt::entity obj = CreateWorldObject(registry, name, false);

        registry.emplace<VelocityComponent>(obj);
        registry.emplace<InputChannelComponent>(obj);
        registry.emplace<MovementComponent>(obj);

        registry.emplace<CharacterTag>(obj);
        registry.emplace<RollbackEntityTag>(obj);

        const entt::entity colObj = registry.get<HierarchyComponent>(obj).children[0];
        const entt::entity colGnd = AttachColliderEntity(registry, colObj, true, "Ground trigger");

        registry.emplace<GroundTriggerTag>(colGnd);

        auto& trfColGnd = registry.get<TransformComponent>(colGnd);
        constexpr float deltaPosY = (1 - Character::GROUND_TRIGGER_SCALE) * 0.5f
                                  + COLLISION_SKIN * Character::GROUND_TRIGGER_EXTENSION_RATIO;
        trfColGnd.SetScale(Character::GROUND_TRIGGER_SCALE);
        trfColGnd.SetPosition(0, -deltaPosY, 0);

        if (printInfo)
        {
            TMT_INFO << "[ Character ] " << std::left << std::setw(12) << name << "\n"
                     << "              entity   ID: " << std::right << std::setw(4) << (int)obj
                     << "              collider ID: " << std::right << std::setw(4) << (int)colObj
                     << "              trigger  ID: " << std::right << std::setw(4) << (int)colGnd;
        }
        return obj;
    }

    entt::entity AttachColliderEntity(entt::registry& registry, entt::entity parent, bool trigger, const std::string& name)
    {
        const entt::entity col = CreateBaseEntity(registry, name, false);

        registry.emplace<ColliderComponent>(col, trigger);
        //registry.emplace<LifetimeComponent>(col);
        registry.emplace<RenderComponent>(col);
        SetHierarchy(registry, parent, col);

        return col;
    }

    /////////////// 구버전
    entt::entity CreateSkybox(entt::registry& reg)
    {
        const entt::entity obj = reg.create();

        auto& generator = reg.ctx().get<EntityNameGenerator>();
        reg.emplace<NametagComponent>(obj, GenerateUUID(), generator.Generate("Skybox"));
        reg.emplace<TransformComponent>(obj);
        reg.emplace<VisibilityComponent>(obj);
        reg.emplace<RootEntityTag>(obj);

        return obj;
    }

    entt::entity CreateGizmo(entt::registry& reg)
    {
        // Center(root)
        const entt::entity center = reg.create();

        auto& generator = reg.ctx().get<EntityNameGenerator>();
        reg.emplace<NametagComponent>(center, GenerateUUID(), generator.Generate("Gizmo"));
        reg.emplace<TransformComponent>(center);
        reg.emplace<VisibilityComponent>(center);
        reg.emplace<RootEntityTag>(center);
        reg.emplace<EditorHidden>(center);
        reg.emplace<RenderComponent>(center,
            glm::vec4(1.f),
            GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube)),
            GetAssetID(Shader::PrimitiveName),
            GetAssetID(Texture::PrimitiveName));

        // X axis
        const entt::entity x = reg.create();
        reg.emplace<NametagComponent>(x, GenerateUUID(), generator.Generate("X"));
        reg.emplace<TransformComponent>(x,
            glm::vec3(3, 0, 0),
            glm::vec3(0, 0, 90),
            glm::vec3(2.5, 5, 2.5));
        reg.emplace<VisibilityComponent>(x);
        reg.emplace<RenderComponent>(x,
            glm::vec4(1.f, 0.f, 0.f, 1.f),
            GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cone)),
            GetAssetID(Shader::PrimitiveName),
            GetAssetID(Texture::PrimitiveName));
        SetHierarchy(reg, center, x);

        // Y axis
        const entt::entity y = reg.create();
        reg.emplace<NametagComponent>(y, GenerateUUID(), generator.Generate("Y"));
        reg.emplace<TransformComponent>(y,
            glm::vec3(0, 3, 0),
            glm::vec3(-180, 0, 0),
            glm::vec3(2.5, 5, 2.5));
        reg.emplace<VisibilityComponent>(y);
        reg.emplace<RenderComponent>(y,
            glm::vec4(0.f, 1.f, 0.f, 1.f),
            GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cone)),
            GetAssetID(Shader::PrimitiveName),
            GetAssetID(Texture::PrimitiveName));
        SetHierarchy(reg, center, y);

        // Z axis
        const entt::entity z = reg.create();
        reg.emplace<NametagComponent>(z, GenerateUUID(), generator.Generate("Z"));
        reg.emplace<TransformComponent>(z,
            glm::vec3(0, 0, 3),
            glm::vec3(-90, 0, 0),
            glm::vec3(2.5, 5, 2.5));
        reg.emplace<VisibilityComponent>(z);
        reg.emplace<RenderComponent>(z,
            glm::vec4(0.f, 0.f, 1.f, 1.f),
            GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cone)),
            GetAssetID(Shader::PrimitiveName),
            GetAssetID(Texture::PrimitiveName));
        SetHierarchy(reg, center, z);

        reg.get<TransformComponent>(center).SetScale(0.1f, 0.1f, 0.1f);
        return center;
    }

    // entt::entity AttachCollider(entt::registry& reg, entt::entity parent, ColliderType type) {
    //     const entt::entity col = reg.create();
    //
    //     auto& generator = reg.ctx().get<EntityNameGenerator>();
    //     reg.emplace<NametagComponent>(col, GenerateUUID(), generator.Generate("Collider"));
    //
    //     SetHierarchy(reg, parent, col);
    //
    //     reg.emplace<TransformComponent>(col);
    //     reg.emplace<ColliderComponent>(col, type);
    //     reg.emplace<RenderComponent>(col,
    //                                  glm::vec4(1.f),
    //                                  GetAssetID(Mesh::GetPrimitiveName(
    //                                          type == ColliderType::Cube ? Mesh::Primitive::Cube : Mesh::Primitive::Sphere)),
    //                                  GetAssetID(Shader::PrimitiveName),
    //                                  GetAssetID(Texture::PrimitiveName));
    //     reg.emplace<VisibilityComponent>(col);
    //
    //     TMT_INFO << "Create collider: " << (int)col;
    //     return col;
    // }
    //
    // entt::entity AttachCharacterCollider(entt::registry& reg, entt::entity parent, ColliderType type) {
    //     const entt::entity col = AttachCollider(reg, parent, type);
    //     TransformComponent& trfP = reg.get<TransformComponent>(parent);
    //
    //     const entt::entity ground = AttachCollider(reg, col, type);
    //     TransformComponent& trfC = reg.get<TransformComponent>(ground);
    //
    //     trfC.SetScale(trfP.GetLocalScale() * 0.8f);
    //     trfC.AddPosition({0.f, -(trfP.GetLocalScale().y * 0.1 + COLLISION_SKIN + 0.001f), 0.f});
    //
    //     reg.get<ColliderComponent>(ground).trigger = true;
    //
    //     TMT_INFO << "Create character collider, ground: " << (int)col << ", " << (int)ground;
    //     return col;
    // }
}
#include <entt/entt.hpp>
#include "Prefab/Prefab.h"
#include "GameObject/Character/CharacterConfig.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/ActiveTag.h"
#include "ECS/Components/Visibility.h"
#include "ECS/Components/Gizmo.h"
#include "ECS/Entity/Hierarchy.h"
#include "ECS/Entity/Entity.h"
#include "Resource/AssetHash.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "Utils/Logger.h"

namespace tomato::Prefab
{
    entt::entity CreateBaseEntity(
        entt::registry& registry,
        bool active, bool root,
        const std::string& name)
    {
        const entt::entity obj = registry.create();

        registry.emplace<NametagComponent>(obj, GenerateUUID(), registry.ctx().get<EntityNameGenerator>().Generate(name));
        registry.emplace<VisibilityComponent>(obj);
        registry.emplace<TransformComponent>(obj);
        if (active)
            registry.emplace<ActiveTag>(obj);
        if (root)
            registry.emplace<RootEntityTag>(obj);

        return obj;
    }

    entt::entity CreateCamera(
        entt::registry& registry,
        bool active, const bool main,
        const std::string& name,
        const glm::vec3& pos, const glm::vec3& rot)
    {
        const entt::entity obj = CreateBaseEntity(registry, active, true, name);

        registry.emplace<CameraComponent>(obj);
        if (main)
            registry.emplace<MainCameraTag>(obj);

        auto& trf = registry.get<TransformComponent>(obj);
        trf.SetPosition(pos);
        trf.SetRotationDegree(rot);

        return obj;
    }

    entt::entity CreateStaticMesh(
        entt::registry& registry,
        bool active,
        const std::string& name)
    {
        const entt::entity obj = CreateBaseEntity(registry, active, true, name);

        registry.emplace<RenderComponent>(obj);

        return obj;
    }

    entt::entity CreateTriggerVolume(
        entt::registry& registry,
        bool active,
        const std::string& name)
    {
        const entt::entity obj = CreateBaseEntity(registry, active, true, name);
        const entt::entity col = AttachColliderEntity(registry, obj, active, true);

        return obj;
    }

    entt::entity CreateWorldObject(
        entt::registry& registry,
        bool active, bool trigger,
        bool printInfo,
        const std::string& name)
    {
        const entt::entity obj = CreateStaticMesh(registry, active, name);
        const entt::entity col = AttachColliderEntity(registry, obj, true, trigger);

        if (printInfo)
        {
            TMT_INFO << "[WorldObject] " << std::left << std::setw(12) << name << "\n"
                     << "              entity   ID: " << std::right << std::setw(4) << (int)obj << "\n"
                     << "              collider ID: " << std::right << std::setw(4) << (int)col;
        }
        return obj;
    }

    entt::entity CreateCharacter(
        entt::registry& registry,
        bool active,
        const std::string& name,
        bool printInfo)
    {
        const entt::entity obj = CreateWorldObject(registry, active, false, false, name);

        registry.emplace<VelocityComponent>(obj);
        registry.emplace<InputChannelComponent>(obj);
        registry.emplace<MovementComponent>(obj);
        registry.emplace<CharacterTag>(obj);
        registry.emplace<RollbackEntityTag>(obj);

        const entt::entity colObj = registry.get<HierarchyComponent>(obj).children[0];
//        registry.get<ColliderComponent>(colObj).type = ColliderType::Capsule;
//        registry.get<RenderComponent>(colObj).mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Capsule));
         const entt::entity colGnd = AttachColliderEntity(registry, colObj, true, true, "Ground trigger");

         registry.emplace<GroundTriggerTag>(colGnd);

         auto& trfColGnd = registry.get<TransformComponent>(colGnd);
         trfColGnd.SetScale(Character::GROUND_TRIGGER_SCALE);

         if (printInfo)
         {
             TMT_INFO << "[ Character ] " << std::left << std::setw(12) << name << "\n"
                     << "              entity   ID: " << std::right << std::setw(4) << (int)obj << "\n"
                     << "              collider ID: " << std::right << std::setw(4) << (int)colObj << "\n"
                     << "              trigger  ID: " << std::right << std::setw(4) << (int)colGnd;
         }
        return obj;
    }

    entt::entity AttachColliderEntity(
        entt::registry& registry, entt::entity parent,
        bool active, bool trigger,
        const std::string& name)
    {
        const entt::entity col = CreateBaseEntity(registry, active, false, name);

        registry.emplace<ColliderComponent>(col, trigger);
        registry.emplace<RenderComponent>(col);
        SetHierarchy(registry, parent, col);

        return col;
    }

    entt::entity CreateSkybox(entt::registry& registry)
    {
        const entt::entity obj = CreateStaticMesh(registry, true, "Skybox");

        registry.emplace<NoInspector>(obj);

        auto& render = registry.get<RenderComponent>(obj);
        render.shader = GetAssetID("SkyboxShader");
        render.texture = GetAssetID("PrimitiveSkybox");
        render.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube));
        render.priority = RenderPriority::Skybox;

        return obj;
    }

    entt::entity CreateGizmo(entt::registry& registry)
    {
        // Center(root)
        const entt::entity center = CreateStaticMesh(registry, true, "Gizmo");

        registry.emplace<GizmoTag>(center);

        // X axis
        const entt::entity x = CreateStaticMesh(registry, true, "X");

        registry.emplace<GizmoTag>(x);

        auto& trfX = registry.get<TransformComponent>(x);
        trfX.SetPosition(3, 0, 0);
        trfX.SetRotationDegree(0, 0, 90);
        trfX.SetScale(2.5, 5, 2.5);

        auto& rndrX = registry.get<RenderComponent>(x);
        rndrX.color = glm::vec4(1.f, 0.f, 0.f, 1.f);
        rndrX.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cone));

        SetHierarchy(registry, center, x);

        // Y axis
        const entt::entity y = CreateStaticMesh(registry, true, "Y");

        registry.emplace<GizmoTag>(y);

        auto& trfY = registry.get<TransformComponent>(y);
        trfY.SetPosition(0, 3, 0);
        trfY.SetRotationDegree(-180, 0, 0);
        trfY.SetScale(2.5, 5, 2.5);

        auto& rndrY = registry.get<RenderComponent>(y);
        rndrY.color = glm::vec4(0.f, 1.f, 0.f, 1.f);
        rndrY.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cone));

        SetHierarchy(registry, center, y);

        // Z axis
        const entt::entity z = CreateStaticMesh(registry, true, "Z");

        registry.emplace<GizmoTag>(z);

        auto& trfZ = registry.get<TransformComponent>(z);
        trfZ.SetPosition(0, 0, 3);
        trfZ.SetRotationDegree(-90, 0, 0);
        trfZ.SetScale(2.5, 5, 2.5);

        auto& rndrZ = registry.get<RenderComponent>(z);
        rndrZ.color = glm::vec4(0.f, 0.f, 1.f, 1.f);
        rndrZ.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cone));

        SetHierarchy(registry, center, z);

        registry.get<TransformComponent>(center).SetScale(0.1f, 0.1f, 0.1f);
        std::cout << "gizmo: " << (int)center << " " << (int)x << " " << (int)y << " " << (int)z << "\n";
        return center;
    }
}
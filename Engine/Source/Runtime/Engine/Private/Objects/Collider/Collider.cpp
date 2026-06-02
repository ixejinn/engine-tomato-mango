#include "Objects/Collider/Collider.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Hierarchy.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"

namespace tomato {
    Collider::Collider(entt::registry& reg, entt::entity parent) : Object(reg) {
        // reg.emplace<ColliderComponent>(entity_,
        //     glm::vec4(1.f),
        //     GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube)),
        //     GetAssetID(Shader::PrimitiveName),
        //     GetAssetID(Texture::PrimitiveName));
        //
        // HierarchyComponent& hc = reg.emplace<HierarchyComponent>(entity_);
        // hc.parent = parent;
    }
}
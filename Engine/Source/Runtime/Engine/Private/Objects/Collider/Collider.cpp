#include "Objects/Collider/Collider.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Render.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"

namespace tomato {
    Collider::Collider(entt::registry& reg, entt::entity parent) : Object(reg) {
        // ColliderComponent& col = reg.emplace<ColliderComponent>(entity_);
        // RenderComponent& render = reg.emplace<RenderComponent>(entity_);
        //
        // HierarchyComponent& hierarchy = reg.emplace<HierarchyComponent>(entity_);
        // hierarchy.parent = parent;
    }
}
#include "Objects/Object.h"
#include "Objects/Collider/Collider.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"

namespace tomato {
    Object::Object(entt::registry& reg) {
        entity_ = reg.create();
        reg.emplace<TransformComponent>(entity_);
        reg.emplace<RenderComponent>(entity_);

        Collider(reg, entity_);
    }
}
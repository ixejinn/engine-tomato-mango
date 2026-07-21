#include "ECS/Systems/UITransformSystem.h"

#include "ECS/Components/Transform.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/UI.h"

#include "ECS/Components/Camera.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "ECS/Entity/Entity.h"

#include "Utils/RegistryEntry.h"
REGISTER_BUILT_IN_SYSTEM(tomato::SystemPhase::UITransformation, UITransformSystem)

namespace tomato
{
	void UITransformSystem::Update(SimContext& simCtx)
	{
		UpdateScreenUI(simCtx);
	}

    void UITransformSystem::UpdateScreenUI(SimContext& simCtx)
    {
        auto& r = simCtx.state->GetRegistry();
        auto* uiCtx = r.ctx().find<UIContext>();
        if (uiCtx == nullptr)
        {
            std::cout << "NULL DRAWLIST\n";
            r.ctx().emplace<UIContext>();
            uiCtx = r.ctx().find<UIContext>();
        }

        for (auto e : uiCtx->drawList)
        {
            auto& hierarchy = r.get<HierarchyComponent>(e);
            auto& rect = r.get<RectTransformComponent>(e);
            auto& ui = r.get<UIComponent>(e);

            // Scale → Rotate → Translate
            auto T = glm::translate(glm::mat4(1.f), rect.position);
            auto R = glm::toMat4(glm::quat(glm::radians(rect.rotation)));
            auto S = glm::scale(glm::mat4(1.f), rect.scale);

            rect.local_matrix = T * R * S;

            if (GetEntityByUUID(r, ui.canvas) == e)
                rect.world_matrix = rect.local_matrix;

            else if (hierarchy.parent != entt::null)
            {
                auto* parentRect = r.try_get<RectTransformComponent>(hierarchy.parent);
                if(parentRect)
                //std::cout << "child rect " << ui.type << " pos(" << rect.position.x << ", " << rect.position.y << ")\n";
                    rect.world_matrix = parentRect->world_matrix * rect.local_matrix;
            }
            else
                rect.world_matrix = rect.local_matrix;

            glm::mat4 renderM = rect.world_matrix;
            if (ui.type != UIType::Text)
            {
                renderM = glm::scale(renderM, glm::vec3(rect.computedSize, 1.0f));
                renderM = glm::translate(renderM, glm::vec3(-rect.pivot, 0.f));
            }
            rect.model_matrix = renderM;
        }
    }
}
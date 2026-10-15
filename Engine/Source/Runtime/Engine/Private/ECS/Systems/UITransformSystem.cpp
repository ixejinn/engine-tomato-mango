#include "ECS/Systems/UITransformSystem.h"

#include "ECS/Components/Transform.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/UI.h"
#include "ECS/Components/Target.h"
#include "ECS/Components/UIDirty.h"

#include "ECS/Components/Camera.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "ECS/Entity/Entity.h"

#include "Utils/Bitmask/BitmaskOperators.h"
#include "Profiler/CPUProfiler.h"
namespace tomato
{
	void UITransformSystem::Update(SimContext& simCtx)
	{
        CPU_PROFILER_BLOCK_BEGIN(UITransformSystem::Update);

        auto& registry = simCtx.state->GetRegistry();

		UpdateScreenUI(registry);
        UpdateWorldUI(registry);

        CPU_PROFILER_BLOCK_END(UITransformSystem::Update);
	}

    void UITransformSystem::UpdateScreenUI(entt::registry& registry)
    {
        auto* uiCtx = registry.ctx().find<UIContext>();
        if (uiCtx == nullptr)
            uiCtx = &registry.ctx().emplace<UIContext>();

        for (auto e : uiCtx->screenDrawList)
        {
            auto& rect = registry.get<RectTransformComponent>(e);
            if(!HasFlag(rect.dirty, UI::Dirty::Matrix))
                continue;

            auto& hierarchy = registry.get<HierarchyComponent>(e);
            auto& ui = registry.get<UIComponent>(e);

            // Scale → Rotate → Translate
            auto T = glm::translate(glm::mat4(1.f), rect.position);
            auto R = glm::toMat4(glm::quat(glm::radians(rect.rotation)));
            auto S = glm::scale(glm::mat4(1.f), rect.scale);

            rect.local_matrix = T * R * S;

            if (GetEntityByUUID(registry, ui.canvas) == e)
                rect.world_matrix = rect.local_matrix;

            else if (hierarchy.parent != entt::null)
            {
                auto* parentRect = registry.try_get<RectTransformComponent>(hierarchy.parent);
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

            rect.dirty = UI::Dirty::None;
        }
    }
    void UITransformSystem::UpdateWorldUI(entt::registry& registry)
    {
        auto* uiCtx = registry.ctx().find<UIContext>();
        if (!uiCtx)
            return;

        for (auto e : uiCtx->worldDrawList)
        {
            auto& rect = registry.get<RectTransformComponent>(e);
            auto& ui = registry.get<UIComponent>(e);

            // Scale → Rotate → Translate
            auto T = glm::translate(glm::mat4(1.f), rect.position);
            auto R = glm::toMat4(glm::quat(glm::radians(rect.rotation)));
            auto S = glm::scale(glm::mat4(1.f), rect.scale);

            rect.local_matrix = T * R * S;

            //World Canvas
            if (GetEntityByUUID(registry, ui.canvas) == e)
            {
                rect.world_matrix = rect.local_matrix;
                rect.model_matrix = rect.world_matrix;
                continue;
            }

            //Target-based World UI
            if (auto* target = registry.try_get<TargetComponent>(e))
            {
                auto targetEntity = GetEntityByUUID(registry, target->target);
                auto* targetTransform = registry.try_get<TransformComponent>(targetEntity);
                if (!targetTransform)
                    continue;

                auto& renderCtx = registry.ctx().get<RenderContext>();
                auto* camera = registry.try_get<CameraComponent>(renderCtx.mainCam);

                if (!camera) continue;

                //Billboard
                glm::mat4 billboard{ 1.f };

                billboard[0] = glm::vec4(camera->right, 0.f);
                billboard[1] = glm::vec4(camera->up, 0.f);
                billboard[2] = glm::vec4(camera->back, 0.f);
                billboard[3] = glm::vec4(rect.position, 1.f);

                rect.world_matrix = billboard;
                rect.model_matrix = rect.world_matrix;

                continue;
            }

            // @TODO: Normal World UI hierarchy
        }
    }
}
#include "ECS/Systems/TransformSystem.h"

#include "ECS/Components/Transform.h"
#include "ECS/Components/UI.h"

#include "ECS/SystemUpdateContexts.h"

#include "Utils/RegistryEntry.h"
REGISTER_SYSTEM(tomato::SystemPhase::Physics, TransformSystem)

namespace tomato {
    void TransformSystem::Update(SimContext &simCtx) {
        auto view = simCtx.registry.view<TransformComponent>();

        for (auto [e, trf] : view.each()) {
            // Scale → Rotate → Translate
            auto T = glm::translate(glm::mat4(1.f), trf.position);
            if (trf.rotDirty)
            {
                trf.rotation = glm::quat(glm::radians(trf.eulerDegree));
                trf.rotDirty = false;
            }
            auto R = glm::toMat4(trf.rotation);
            auto S = glm::scale(glm::mat4(1.f), trf.scale);

            trf.transformMatrix = T * R * S;
        }

        UpdateScreenUI(simCtx);
    }
    void TransformSystem::UpdateScreenUI(SimContext& simCtx)
    {
        auto& r = simCtx.registry;
        auto& uiCtx = r.ctx().get<UIContext>();
        for (auto e : uiCtx.drawList)
        {
            auto& hierarchy = r.get<HierarchyComponent>(e);
            auto& rect = r.get<RectTransformComponent>(e);
            auto& ui = r.get<UIComponent>(e);

            // Scale → Rotate → Translate
            auto T = glm::translate(glm::mat4(1.f), rect.position);
            auto R = glm::toMat4(glm::quat(glm::radians(rect.rotation)));
            auto S = glm::scale(glm::mat4(1.f), rect.scale);

            rect.local_matrix = T * R * S;

            if (ui.canvas == e)
                rect.world_matrix = rect.local_matrix;

            else if (hierarchy.parent != entt::null)
            {
                auto& parentRect = r.get<tomato::RectTransformComponent>(hierarchy.parent);

                //std::cout << "child rect " << ui.type << " pos(" << rect.position.x << ", " << rect.position.y << ")\n";
                rect.world_matrix = parentRect.world_matrix * rect.local_matrix;
            }
            else
                rect.world_matrix = rect.local_matrix;

            glm::mat4 renderM = rect.world_matrix;
            if (ui.type != 2)
            {
                renderM = glm::scale(renderM, glm::vec3(rect.computedSize, 1.0f));
                renderM = glm::translate(renderM, glm::vec3(-rect.pivot, 0.f));
            }
            rect.model_matrix = renderM;
        }
    }
}
#include "ECS/Systems/TransformSystem.h"

#include "ECS/Components/Transform.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/UI.h"

#include "ECS/Components/Rigidbody.h"
#include "ECS/Components/Camera.h"
#include "ECS/SystemUpdateContexts.h"

#include "Utils/RegistryEntry.h"
REGISTER_SYSTEM(tomato::SystemPhase::Transformation, TransformSystem)

namespace tomato {
    void TransformSystem::Update(SimContext &simCtx) {
        auto rootView = simCtx.registry.view<RootEntityTag, TransformComponent>();

        static constexpr glm::quat rootQuat = glm::quat(1.f, 0.f, 0.f, 0.f);
        static constexpr glm::vec3 rootScale = glm::vec3{1.f};
        static constexpr glm::mat4 rootMat = glm::mat4{1.f};

        for (auto [e, trf] : rootView.each()) {
            UpdateFrom(simCtx.registry, e, rootQuat, rootScale, rootMat, false);
            // auto pos = trf.GetWorldPosition();
            // TMT_INFO << (int)e << " world pos: " << pos.x << " " << pos.y << " " << pos.z;
        }

        UpdateScreenUI(simCtx);
    }

    void TransformSystem::UpdateFrom(
        entt::registry& reg, entt::entity cur,
        const glm::quat& pQuat, const glm::vec3& pScale,
        const glm::mat4& pMatrix, bool pDirty)
    {
        auto& trf = reg.get<TransformComponent>(cur);

        bool bUpdated = trf.dirty || pDirty;
        if (bUpdated) {
            // Scale → Rotate → Translate
            auto T = glm::translate(glm::mat4(1.f), trf.position);
            auto R = glm::toMat4(trf.lRotation);
            auto S = glm::scale(glm::mat4(1.f), trf.scale);

            trf.wRotation = pQuat * trf.lRotation;
            trf.wScale = pScale * trf.scale;
            trf.transformMatrix = pMatrix * (T * R * S);
            trf.dirty = false;

            if (auto* cam = reg.try_get<CameraComponent>(cur))
                cam->dirty = true;
        }

        if (auto* hierarchy = reg.try_get<HierarchyComponent>(cur)) {
            for (const auto child : hierarchy->children)
                UpdateFrom(reg, child, trf.wRotation, trf.wScale, trf.transformMatrix, bUpdated);
        }
    }

    void TransformSystem::UpdateScreenUI(SimContext& simCtx)
    {
        auto& r = simCtx.registry;
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
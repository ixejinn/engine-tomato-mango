#include "ECS/Systems/UISystem.h"
#include "ECS/SystemUpdateContexts.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/UI.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Text.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Camera.h"
#include "Services/Window.h"
#include "Services/Input.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Render/Font.h"
#include "Prefab/EntityUtils.h"
#include "Utils/Utf.h"
#include "Utils/Logger.h"

#include "Utils/RegistryEntry.h"
REGISTER_SYSTEM(tomato::SystemPhase::UI, UISystem)

namespace tomato
{
	UISystem::UISystem() {}

	void UISystem::Update(SimContext& ctx)
	{
		BuildDrawList(ctx);

		UpdateTextContentSize(ctx);
		UpdateRectTransform(ctx);

		BulidSelectableList(ctx);
	}

	void UISystem::Traverse(SimContext& ctx, entt::entity e, std::vector<entt::entity>& drawList)
	{
		auto& registry = ctx.state->GetRegistry();
		auto& ui = registry.get<UIComponent>(e);
		//std::cout << ui.type << " ";
		drawList.push_back(e);

		auto& hierarchy = registry.get<HierarchyComponent>(e);
		for (auto child : hierarchy.children)
			Traverse(ctx, child, drawList);
	}

	void UISystem::BuildDrawList(SimContext& ctx)
	{
		auto& registry = ctx.state->GetRegistry();
		auto* uiCtx = registry.ctx().find<UIContext>();
		if (uiCtx == nullptr)
		{
			std::cout << "NULL DRAWLIST\n";
			registry.ctx().emplace<UIContext>();
			uiCtx = registry.ctx().find<UIContext>();
		}

		std::vector<entt::entity> canvases, drawList;

		auto canvasView = registry.view<CanvasComponent>();
		for (auto canvas : canvasView)
			canvases.push_back(canvas);

		std::sort(canvases.begin(), canvases.end(),
			[&](entt::entity a, entt::entity b)
			{
				return registry.get<CanvasComponent>(a).sortOrder <
					registry.get<CanvasComponent>(b).sortOrder;
			});

		for (auto canvas : canvases)
			Traverse(ctx, canvas, drawList);

		std::stable_sort(
			drawList.begin(),
			drawList.end(),
			[&](entt::entity a, entt::entity b)
			{
				auto& uiA = registry.get<UIComponent>(a);
				auto& uiB = registry.get<UIComponent>(b);

				return uiA.sortOrder < uiB.sortOrder;
			}
		);

		uiCtx->drawList.clear();
		uiCtx->drawList = std::move(drawList);
	}

	void UISystem::BulidSelectableList(SimContext& ctx)
	{
		auto& registry = ctx.state->GetRegistry();
		auto* uiCtx = registry.ctx().find<UIContext>();

		if (uiCtx == nullptr)
			return;

		if (!uiCtx->selectableDirty) return;

		for (auto it = uiCtx->drawList.rbegin(); it != uiCtx->drawList.rend(); ++it)
		{
			if (!registry.all_of<SelectableComponent>(*it)) continue;
			uiCtx->selectableList.emplace_back(*it);
		}

		uiCtx->selectableDirty = false;
	}

	void UISystem::UpdateTextContentSize(SimContext& ctx)
	{
		auto& registry = ctx.state->GetRegistry();
		auto textView = registry.view<UIComponent, RectTransformComponent, TextComponent>();
		for (auto [e, ui, rect, text] : textView.each())
		{
			if (ui.type == UIType::Text)
			{
				auto& text = registry.get<TextComponent>(e);
				if (text.dirty)
				{
					text.codepoints = UTF8ToUTF32(text.text);
					Font* font = AssetRegistry<Font>::GetInstance().Get(text.font);

					rect.sizeDelta = font->MeasureText(text.codepoints, text.fontSize / 64.f);
					text.dirty = false;
				}
			}
		}
	}

	void UISystem::UpdateRectTransform(SimContext& ctx)
	{
		auto& registry = ctx.state->GetRegistry();
		auto& uiCtx = registry.ctx().get<UIContext>();

		if (uiCtx.drawList.empty())
			return;

		CanvasComponent* currentCanvas = nullptr;
		for (auto entity : uiCtx.drawList)
		{
			auto& hierarchy = registry.get<HierarchyComponent>(entity);

			// entity is canvas(root).
			if (hierarchy.parent == entt::null)
			{
				currentCanvas = &registry.get<CanvasComponent>(entity);

				auto& rect = registry.get<RectTransformComponent>(entity);
				rect.computedSize = currentCanvas->actualSize;
				rect.position = glm::vec3(rect.computedSize * rect.pivot, 0.f);
				rect.scale = glm::vec3(1.f);

				rect.screenPosition = rect.position;
				break;
			}
		}

		if (!currentCanvas) return;
		for (auto entity : uiCtx.drawList)
		{
			auto& hierarchy = registry.get<HierarchyComponent>(entity);
			if (hierarchy.parent == entt::null)
				continue;

			// children
			auto& rect = registry.get<RectTransformComponent>(entity);
			auto& parentRect = registry.get<RectTransformComponent>(hierarchy.parent);
			//auto& ui = ctx.registry.get<UIComponent>(entity);

			glm::vec2 scaleFactor = currentCanvas->actualSize / currentCanvas->referenceSize;
			glm::vec2 parentSize = parentRect.computedSize;
			glm::vec2 parentPivotPos = parentSize * parentRect.pivot;

			if (rect.anchorMin == rect.anchorMax) // anchor point
			{
				glm::vec2 anchorPos = parentSize * rect.anchorMin;
				glm::vec2 localPos = (anchorPos - parentPivotPos) + rect.anchoredPosition;

				// World Name Label
				if (registry.all_of<TargetComponent>(entity))
				{
					auto& target = registry.get<TargetComponent>(entity);
					auto& targetTransform = registry.get<TransformComponent>(GetEntityByUUID(registry, target.target));

					// if (!ctx.registry.ctx().get<RenderContext*>())
					// 	return;
					auto& renderCtx = registry.ctx().get<RenderContext>();
					if (renderCtx.mainCam == entt::null)
					{
						TMT_WARN << "Main camera not present";
						continue;
					}
					auto viewProjMat = registry.try_get<CameraComponent>(renderCtx.mainCam)->viewProjMat;

					glm::vec3 screenPos = WorldToScreen(targetTransform.GetWorldPosition(), viewProjMat, Window::GetWidth(), Window::GetHeight());
					rect.position = screenPos + target.headOffset;

					rect.computedSize = rect.sizeDelta;
					continue;
				}

				rect.computedSize = rect.sizeDelta;
				rect.position = glm::vec3(localPos * scaleFactor, 0.f);
			}
			else // anchor stretch
			{
				glm::vec2 anchorPosMin = parentSize * rect.anchorMin;
				glm::vec2 anchorPosMax = parentSize * rect.anchorMax;

				glm::vec2 finalLocalMin = (anchorPosMin - parentPivotPos) + rect.offsetMin;
				glm::vec2 finalLocalMax = (anchorPosMax - parentPivotPos) + rect.offsetMax;

				rect.computedSize = finalLocalMax - finalLocalMin;

				glm::vec2 localPos = finalLocalMin + (rect.computedSize * rect.pivot);
				rect.position = glm::vec3(localPos * scaleFactor, 0.f);
			}

			rect.screenPosition = parentRect.screenPosition + rect.position;
			rect.screenRect.min.x = rect.screenPosition.x - (rect.computedSize.x * rect.pivot.x);
			rect.screenRect.min.y = rect.screenPosition.y - (rect.computedSize.y * rect.pivot.y);

			rect.screenRect.max.x = rect.screenRect.min.x + rect.computedSize.x;
			rect.screenRect.max.y = rect.screenRect.min.y + rect.computedSize.y;
		}
	}

	glm::vec3 UISystem::WorldToScreen(const glm::vec3& worldPos, const glm::mat4& viewProjection, float screenWidth, float screenHeight)
	{
		//World -> Clip
		glm::vec4 clipPos = viewProjection * glm::vec4(worldPos, 1.0f);

		//Perspective divide (Clip -> NDC)
		glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;

		//NDC -> Screen
		glm::vec2 screenPos{};
		screenPos.x = (ndc.x * 0.5f + 0.5f) * screenWidth;
		screenPos.x -= screenWidth * 0.5f;

		screenPos.y = (ndc.y * 0.5f + 0.5f) * screenHeight;
		screenPos.y -= screenHeight * 0.5f;

		return glm::vec3(screenPos, 0.f);
	}
}

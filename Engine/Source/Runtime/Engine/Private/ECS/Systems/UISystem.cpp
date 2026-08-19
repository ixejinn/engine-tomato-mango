#include "ECS/Systems/UISystem.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"

#include "ECS/Components/Transform.h"
#include "ECS/Components/UI.h"
#include "ECS/Components/Target.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Text.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Camera.h"
#include "ECS/Entity/Entity.h"

#include "Services/Window.h"
#include "Services/Input.h"
#include "Event/EventDispatcher.h"
#include "Services/ChangeFramebufferSizeEvent.h"

#include "Resource/AssetRegistry.h"
#include "Resource/Render/Font.h"

#include "Utils/Utf.h"
#include "Utils/Logger.h"

namespace tomato
{
	UISystem::UISystem()
	{
		EventDispatcher::GetInstance().Connect<ChangeFramebufferSizeEvent, &UISystem::OnChangeWindowSize>(*this);
	}

	void UISystem::Update(SimContext& ctx)
	{
		if (this->ctx != &ctx)
			this->ctx = &ctx;

		BuildDrawList(ctx);

		UpdateTextContentSize(ctx);
		UpdateScreenRectTransform(ctx);
		UpdateWorldRectTransform(ctx);

		BulidSelectableList(ctx);
	}

	void UISystem::OnChangeWindowSize()
	{
		auto& registry = ctx->state->GetRegistry();
		auto canvasView = registry.view<CanvasComponent>();

		for (auto [e, canvas] : canvasView.each())
			canvas.actualSize = { Window::GetWidth(), Window::GetHeight() };
	}

	void UISystem::Traverse(SimContext& ctx, entt::entity e, std::vector<entt::entity>& drawList)
	{
		auto& registry = ctx.state->GetRegistry();
		auto* ui = registry.try_get<UIComponent>(e);
		if (!ui) return;

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

		std::vector<entt::entity> canvases, screenDrawList, worldDrawList;

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
		{
			auto& canvasCmp = registry.get<CanvasComponent>(canvas);
			if (canvasCmp.mode == RenderMode::ScreenOverlay)
				Traverse(ctx, canvas, screenDrawList);
			else
				Traverse(ctx, canvas, worldDrawList);
		}

		std::stable_sort(
			screenDrawList.begin(),
			screenDrawList.end(),
			[&](entt::entity a, entt::entity b)
			{
				auto& uiA = registry.get<UIComponent>(a);
				auto& uiB = registry.get<UIComponent>(b);

				return uiA.sortOrder < uiB.sortOrder;
			}
		);

		std::stable_sort(
			worldDrawList.begin(),
			worldDrawList.end(),
			[&](entt::entity a, entt::entity b)
			{
				auto& uiA = registry.get<UIComponent>(a);
				auto& uiB = registry.get<UIComponent>(b);

				return uiA.sortOrder < uiB.sortOrder;
			}
		);

		uiCtx->screenDrawList.clear();
		uiCtx->screenDrawList = std::move(screenDrawList);

		uiCtx->worldDrawList.clear();
		uiCtx->worldDrawList = std::move(worldDrawList);
	}

	void UISystem::BulidSelectableList(SimContext& ctx)
	{
		auto& registry = ctx.state->GetRegistry();
		auto* uiCtx = registry.ctx().find<UIContext>();

		if (uiCtx == nullptr)
			return;

		if (!uiCtx->selectableDirty) return;

		for (auto it = uiCtx->screenDrawList.rbegin(); it != uiCtx->screenDrawList.rend(); ++it)
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

	void UISystem::UpdateScreenRectTransform(SimContext& ctx)
	{
		auto& registry = ctx.state->GetRegistry();
		auto& uiCtx = registry.ctx().get<UIContext>();

		if (uiCtx.screenDrawList.empty())
			return;

		CanvasComponent* currentCanvas = nullptr;
		for (auto entity : uiCtx.screenDrawList)
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
		for (auto entity : uiCtx.screenDrawList)
		{
			auto& hierarchy = registry.get<HierarchyComponent>(entity);
			if (hierarchy.parent == entt::null)
				continue;

			// children
			auto& rect = registry.get<RectTransformComponent>(entity);
			auto& parentRect = registry.get<RectTransformComponent>(hierarchy.parent);

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
					auto* target = registry.try_get<TargetComponent>(entity);
					if (!target) continue;

					auto* targetTransform = registry.try_get<TransformComponent>(GetEntityByUUID(registry, target->target));
					if (!targetTransform)
						continue;

					 if (!registry.ctx().find<RenderContext>())
					 	continue;
					auto renderCtx = registry.ctx().get<RenderContext>();
					if (renderCtx.mainCam == entt::null)

					{
						TMT_WARN << "Main camera not present";
						continue;
					}
					auto viewProjMat = registry.try_get<CameraComponent>(renderCtx.mainCam);

					glm::vec3 screenPos =
						WorldToScreen(
							targetTransform->GetWorldPosition(),
							viewProjMat == nullptr ? glm::mat4(1.f) : viewProjMat->viewProjMat,
							Window::GetWidth(), Window::GetHeight());
					rect.position = screenPos + target->headOffset;

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

	void UISystem::UpdateWorldRectTransform(SimContext& ctx)
	{
		auto& registry = ctx.state->GetRegistry();
		auto& uiCtx = registry.ctx().get<UIContext>();

		if (uiCtx.worldDrawList.empty())
			return;

		/*CanvasComponent* currentCanvas = nullptr;
		for (auto entity : uiCtx.worldDrawList)
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

		if (!currentCanvas) return;*/
		for (auto entity : uiCtx.worldDrawList)
		{
			auto& hierarchy = registry.get<HierarchyComponent>(entity);
			if (hierarchy.parent == entt::null)
				continue;

			// children
			auto& rect = registry.get<RectTransformComponent>(entity);

			// Target-based World UI
				
			if (auto* target = registry.try_get<TargetComponent>(entity))
			{
				auto* targetTransform = registry.try_get<TransformComponent>(GetEntityByUUID(registry, target->target));
				if (!targetTransform) continue;

				rect.position = targetTransform->GetWorldPosition() + target->headOffset;

				rect.computedSize = rect.sizeDelta;
				continue;
			}

			//@TODO : Normal World UI
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

#include "InspectorDraw.h"

#include <entt/entt.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "EditorPanel.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Audio/Audio.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "Resource/Render/Font.h"
#include "Resource/Render/ParticleEffect.h"
#include "Resource/PathManager.h"

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ECS/Components/Components.h"

#include "ECS/Entity/Hierarchy.h"
#include "ECS/Entity/Entity.h"

#include "Utils/FileDialog.h"
namespace tomato
{
	bool DrawCameraInspcetor(EditorContext& eCtx, entt::registry& reg, CameraComponent& camera)
	{
		bool changed = false;

		ImGui::SeparatorText("Mode");

		if (ImGui::RadioButton("Perspective", camera.mode == ProjectionMode::Perspective))
		{
			camera.mode = ProjectionMode::Perspective;
			camera.dirty = true;
			changed = true;
		}

		ImGui::SameLine();

		if (ImGui::RadioButton("Orthogonal", camera.mode == ProjectionMode::Orthogonal))
		{
			camera.mode = ProjectionMode::Orthogonal;
			camera.dirty = true;
			changed = true;
		}

		float fov{ camera.degree }, nearClip{ camera.zNear }, farClip{ camera.zFar };
		if (camera.mode == ProjectionMode::Perspective)
		{
			if(ImGui::SliderFloat("Field Of View", &fov, 1.f, 179.f, "%.2f"))
			{
				camera.dirty = true;
				changed = true;
			}
		}
			
		if(ImGui::DragFloat("Near Clip", &nearClip, 0.1f, 0.01f, 100.f, "%.2f"))
		{
			camera.dirty = true;
			changed = true;
		}
		if(ImGui::DragFloat("Far Clip", &farClip, 0.1f, nearClip, 10000.f, "%.2f"))
		{
			camera.dirty = true;
			changed = true;
		}

		camera.degree = std::clamp(fov, 1.0f, 179.0f);
		camera.zNear = std::max(0.01f, nearClip);
		camera.zFar = std::max(camera.zNear + 0.01f, farClip);

		ImGui::NewLine();

		return changed;
	}

	bool DrawTransformInspector(EditorContext& eCtx, entt::registry& reg, TransformComponent& transform)
	{
		bool changed = false;

		glm::vec3 pos = transform.GetLocalPosition();
		float posVec3[3] = { pos.x, pos.y, pos.z };
		if (DrawVec3Control("Position", posVec3))
		{
			transform.SetPosition(posVec3[0], posVec3[1], posVec3[2]);
			changed = true;
		}

		glm::vec3 degree = transform.GetLocalRotationDegree();
		float degreeVec3[3] = { degree.x, degree.y, degree.z };
		if (DrawVec3Control("Degree", degreeVec3))
		{
			transform.SetRotationDegree(degreeVec3[0], degreeVec3[1], degreeVec3[2]);
			changed = true;
		}

		glm::vec3 scale = transform.GetLocalScale();
		float scaleVec3[3] = { scale.x, scale.y, scale.z };
		if (DrawVec3Control("Scale", scaleVec3, 1e-4f, 0.f, ImGuiSliderFlags_ClampOnInput))
		{
			transform.SetScale(scaleVec3[0], scaleVec3[1], scaleVec3[2]);
			changed = true;
		}

		ImGui::NewLine();

		return changed;
	}

	bool DrawMovementInspector(EditorContext& eCtx, entt::registry& reg, MovementComponent& movement)
	{
		bool changed = false;

		ImGui::NewLine();

		return changed;
	}

	bool DrawVelocityInspector(EditorContext& eCtx, entt::registry& reg, VelocityComponent& vel)
	{
		bool changed = false;

		ImGui::SeparatorText("Speed");
		if (ImGui::DragFloat("##Speed", &vel.horizontalSpeed, 1.0f, 0.f, 100.f, "%.2f"))
			changed = true;

		ImGuiSliderFlags flags = ImGuiSliderFlags_NoInput;
		if (DrawVec3Control("Velocity", glm::value_ptr(vel.velocity), flags))
			changed = true;

		ImGui::NewLine();

		return changed;
	}

	bool DrawColliderInspector(EditorContext& eCtx, entt::registry& reg, ColliderComponent& collider)
	{
		bool changed = false;

		ImGui::SeparatorText("Layer");
		const char* layPreview = "Default";
		if (ImGui::BeginCombo("##Layer", layPreview))
		{
			if (ImGui::Selectable("Default", collider.layer == CollisionLayer::Default))
			{
				collider.layer = CollisionLayer::Default;
				changed = true;
			}

			ImGui::EndCombo();
		}

		ImGui::SeparatorText("Type");
		const char* typePreview = ColliderTypeMetas[(uint8_t)collider.type].name;
		if (ImGui::BeginCombo("##Type", typePreview))
		{
			for (const auto& info : ColliderTypeMetas)
			{
				if (ImGui::Selectable(info.name, collider.type == info.type))
				{
					collider.type = info.type;
					collider.aabbDirty = true;
					changed = true;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::SeparatorText("Trigger");
		ImGui::Text("Is Trigger"); ImGui::SameLine();
		if(ImGui::Checkbox("##Trigger", &collider.isTrigger))
			changed = true;

		ImGui::NewLine();

		return changed;
	}

	bool DrawRenderInspector(EditorContext& eCtx, entt::registry& reg, RenderComponent& render)
	{
		bool changed = false;

		ImGui::SeparatorText("Color");
		ImGui::ColorEdit4("##color", glm::value_ptr(render.color));

		ImGui::SeparatorText("Mesh");
		const char* meshPreview = AssetRegistry<Mesh>::GetInstance().GetName(render.mesh) + 11;

		if (ImGui::BeginCombo("##mesh", meshPreview))
		{
			for (const auto& info : Mesh::PrimitiveMetas)
			{
				if(ImGui::Selectable(
					info.name,
					render.mesh == GetAssetID(Mesh::GetPrimitiveName(info.primitive))))
				{
					render.mesh = GetAssetID(Mesh::GetPrimitiveName(info.primitive));
					changed = true;
				}
			}
			
			ImGui::EndCombo();
		}

		ImGui::SeparatorText("Shader");
		const char* curShader = AssetRegistry<Shader>::GetInstance().GetName(render.shader);
		if (ImGui::BeginCombo("##shader", curShader))
		{
			auto it = AssetRegistry<Shader>::GetInstance().GetNameMapBegin();
			auto endIt = AssetRegistry<Shader>::GetInstance().GetNameMapEnd();
			for (it; it != endIt; it++)
			{
				if (ImGui::Selectable(it->second.c_str(), render.shader == it->first))
				{
					render.shader = it->first;
					changed = true;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::SeparatorText("Texture");
		std::filesystem::path curTex = AssetRegistry<Texture>::GetInstance().GetName(render.texture);
		if (ImGui::BeginCombo("##texture", curTex.filename().string().c_str()))
		{
			auto it = AssetRegistry<Texture>::GetInstance().GetNameMapBegin();
			auto endIt = AssetRegistry<Texture>::GetInstance().GetNameMapEnd();
			for (it; it != endIt; it++)
			{
				curTex = it->second;
				if (ImGui::Selectable(curTex.filename().string().c_str(), render.texture == it->first))
				{
					render.texture = it->first;
					changed = true;
				}
			}
			ImGui::EndCombo();
		}

		auto loadSrc = DrawLoadResourceButton("Open File", "All Images \0*.png;*.jpg;*.jpeg\0\0", PathManager::ProjectResource() / "Img");
		if (!loadSrc.empty())
			render.texture = GetAssetID(loadSrc);

		ImGui::NewLine();

		return changed;
	}

	bool DrawUIInspector(EditorContext& eCtx, entt::registry& reg, UIComponent& ui)
	{
		bool changed = false;

		if (ui.canvas != 0)
		{
			ImGui::SeparatorText("Canvas");
			auto canvasView = reg.view<CanvasComponent, NametagComponent>();
			auto curCanvas = reg.try_get<NametagComponent>(GetEntityByUUID(reg, ui.canvas));
			const char* canvasPreview = curCanvas != nullptr ? curCanvas->name.c_str() : "null";
			if (ImGui::BeginCombo("##canvas", canvasPreview))
			{
				for (auto [e, canvas, tag] : canvasView.each())
				{
					if (ImGui::Selectable(tag.name.c_str(), ui.canvas == tag.id))
					{
						ui.canvas = tag.id;
						SetHierarchy(reg, GetEntityByUUID(reg, ui.canvas), eCtx.selectedEntity);
						changed = true;
					}
				}
				ImGui::EndCombo();
			}
		}

		ImGui::SeparatorText("Sort Order");
		if(ImGui::InputInt("##ui Order", &ui.sortOrder, 0, 1000))
			changed = true;

		ImGui::SeparatorText("UI Type");
		const char* typePreview = UITypeMetas[(int)ui.type].name;
		if (ImGui::BeginCombo("##uitypes", typePreview))
		{
			for (auto info : UITypeMetas)
			{
				if (ImGui::Selectable(info.name, ui.type == info.type, ImGuiSelectableFlags_Disabled))
				{
					ui.type = info.type;
					changed = true;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::NewLine();

		return changed;
	}

	bool DrawCanvasInspector(EditorContext& eCtx, entt::registry& reg, CanvasComponent& canvas)
	{
		bool changed = false;

		ImGui::SeparatorText("Render Mode");
		if (ImGui::BeginCombo("##renderMode", "ScreenOverlay"))
			ImGui::EndCombo();
		
		ImGui::SeparatorText("Size");
		ImGui::Text("Reference Size");// ImGui::SameLine();
		if (ImGui::DragFloat2("##refSize", glm::value_ptr(canvas.referenceSize)))
			changed = true;

		ImGui::Text("Actual Size");// ImGui::SameLine();
		if(ImGui::DragFloat2("##actSize", glm::value_ptr(canvas.actualSize)))
			changed = true;

		ImGui::SeparatorText("Sort Order");
		if(ImGui::InputInt("##canvasOrder", &canvas.sortOrder, 0, 1000))
			changed = true;

		ImGui::NewLine();

		return changed;
	}

	bool DrawRectTransformInspector(EditorContext& eCtx, entt::registry& reg, RectTransformComponent& rect)
	{
		bool changed = false;

		auto ui = reg.try_get<UIComponent>(eCtx.selectedEntity);

		if (rect.anchorMin == rect.anchorMax)
		{
			ImGui::SeparatorText("Position");
			ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(60.f);
			if(ImGui::DragFloat("##anchoredPosx", &rect.anchoredPosition.x, 1.f, 0.f, 0.f, "%g"))
				changed = true; 

			ImGui::SameLine();
			ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(60.f);
			if(ImGui::DragFloat("##anchoredPosy", &rect.anchoredPosition.y, 1.f, 0.f, 0.f, "%g"))
				changed = true;

			if (ui && ui->type != UIType::Text)
			{
				ImGui::SeparatorText("Size");
				ImGui::Text("Width"); ImGui::SameLine(); ImGui::SetCursorPosX(95.f);
				ImGui::Text("Height"); ImGui::SetNextItemWidth(80.f);
				if(ImGui::DragFloat("##sizewidth", &rect.sizeDelta.x, 1.f, 0.f, 0.f, "%g"))
					changed = true;

				ImGui::SameLine(); ImGui::SetNextItemWidth(80.f);
				if(ImGui::DragFloat("##sizeheight", &rect.sizeDelta.y, 1.f, 0.f, 0.f, "%g"))
					changed = true;
			}

			ImGui::SeparatorText("Offset");
			ImGui::Text("Min"); ImGui::SameLine(); ImGui::SetCursorPosX(50.f); ImGui::SameLine();
			ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.f);
			if(ImGui::DragFloat("##offsetMinX", &rect.offsetMin.x, 0.1f, 0.f, 1.f, "%.1f"))
				changed = true;

			ImGui::SameLine();
			ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.f);
			if(ImGui::DragFloat("##offsetMinY", &rect.offsetMin.y, 0.1f, 0.f, 1.f, "%.1f"))
				changed = true;

			ImGui::Text("Max");ImGui::SameLine(); ImGui::SetCursorPosX(40.f); ImGui::SameLine();
			ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.f); ImGui::SameLine();
			if(ImGui::DragFloat("##offsetMaxX", &rect.offsetMax.x, 0.1f, 0.f, 1.f, "%.1f"))
				changed = true; 
			
			ImGui::SameLine();
			ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.f);
			if(ImGui::DragFloat("##offsetMaxY", &rect.offsetMax.x, 0.1f, 0.f, 1.f, "%.1f"))
				changed = true;
		}
		else
		{
			float offsetRight{ std::abs(rect.offsetMax.x) }, offsetTop{ std::abs(rect.offsetMax.y) };

			ImGui::SeparatorText("Margin"); // Offset? Margin?
			ImGui::Text("Left");
			ImGui::SameLine(); ImGui::SetCursorPosX(95.f);
			ImGui::Text("Top");

			ImGui::SetNextItemWidth(80.f);
			if(ImGui::InputFloat("##Left", &rect.offsetMin.x, 0.0f, 0.0f, "%g"))
				changed = true; 
			
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.f);
			if (ImGui::InputFloat("##Top", &offsetTop, 0.0f, 0.0f, "%g"))
			{
				rect.offsetMax.y = -offsetTop;
				changed = true;
			}

			ImGui::Text("Right");
			ImGui::SameLine(); ImGui::SetCursorPosX(95.f);
			ImGui::Text("Bottom");
			ImGui::SetNextItemWidth(80.f);

			if(ImGui::InputFloat("##Right", &offsetRight, 0.0f, 0.0f, "%g"))
			{
				rect.offsetMax.x = -offsetRight;
				changed = true;
			}
			ImGui::SameLine(); ImGui::SetNextItemWidth(80.f);
			if(ImGui::InputFloat("##Bottom", &rect.offsetMin.y, 0.0f, 0.0f, "%g"))
				changed = true;
		}

		ImGui::SeparatorText("Anchor");
		ImGui::Text("Min"); ImGui::SameLine(); ImGui::SetCursorPosX(50.f); ImGui::SameLine();
		ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.f);
		if(ImGui::DragFloat("##anchorMinX", &rect.anchorMin.x, 0.1f, 0.f, 1.f, "%.1f"))
			changed = true; 
		
		ImGui::SameLine();
		ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.f);
		if(ImGui::DragFloat("##anchorMinY", &rect.anchorMin.y, 0.1f, 0.f, 1.f, "%.1f"))
			changed = true;

		ImGui::Text("Max");ImGui::SameLine(); ImGui::SetCursorPosX(40.f); ImGui::SameLine();
		ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.f);
		if(ImGui::DragFloat("##anchorMaxX", &rect.anchorMax.x, 0.1f, 0.f, 1.f, "%.1f"))
			changed = true; 
		
		ImGui::SameLine();
		ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.f);
		if(ImGui::DragFloat("##anchorMaxY", &rect.anchorMax.y, 0.1f, 0.f, 1.f, "%.1f"))
			changed = true;

		ImGui::SeparatorText("Pivot");
		ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(60.f);
		if(ImGui::DragFloat("##pivotx", &rect.pivot.x, 0.1f, 0.f, 1.f, "%.1f"))
			changed = true; 
		
		ImGui::SameLine();
		ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(60.f);
		if(ImGui::DragFloat("##pivoty", &rect.pivot.y, 0.1f, 0.f, 1.f, "%.1f"))
			changed = true;

		ImGui::NewLine();

		return changed;
	}

	bool DrawTextInspector(EditorContext& eCtx, entt::registry& reg, TextComponent& text)
	{
		bool changed = false;

		ImGui::SeparatorText("Text");
		if (ImGui::InputText("##text", &text.text))
		{
			text.dirty = true;
			changed = true;
		}

		ImGui::SeparatorText("Color");
		if(ImGui::ColorEdit4("##textcolor", glm::value_ptr(text.color)))
			changed = true;

		ImGui::SeparatorText("Size");
		if(ImGui::DragFloat("##fontSize", &text.fontSize, 1.f, 0.0f, 0.0f, "%g"))
			changed = true;

		auto beginIt = AssetRegistry<Font>::GetInstance().GetNameMapBegin();
		auto endIt = AssetRegistry<Font>::GetInstance().GetNameMapEnd();

		std::filesystem::path curFont = AssetRegistry<Font>::GetInstance().GetName(text.font);
		if (ImGui::BeginCombo("##fontCombo", curFont.filename().string().c_str()))
		{
			for (beginIt; beginIt != endIt; ++beginIt)
			{
				curFont = beginIt->second;
				if (ImGui::Selectable(curFont.filename().string().c_str(), text.font == beginIt->first))
				{
					text.font = beginIt->first;
					text.dirty = true;
					changed = true;
				}
			}
			ImGui::EndCombo();
		}

		auto loadFont = DrawLoadResourceButton("Open File", "Font files \0*.ttf\0\0", PathManager::ProjectResource() / "Fonts");
		if (!loadFont.empty())
			text.font = GetAssetID(loadFont);

		ImGui::NewLine();

		return changed;
	}

	bool DrawTargetInspector(EditorContext& eCtx, entt::registry& reg, TargetComponent& target)
	{
		bool changed = false;

		ImGui::SeparatorText("Target");
		
		entt::entity targetEntity = GetEntityByUUID(reg, target.target);
		const char* namePreview = targetEntity == entt::null ? "None" : reg.try_get<NametagComponent>(targetEntity)->name.c_str();
		if (ImGui::BeginCombo("##entityCombo", namePreview))
		{
			auto view = reg.view<NametagComponent, TransformComponent>();
			for (auto [e, tag, transform] : view.each())
			{
				if (ImGui::Selectable(tag.name.c_str(), target.target == tag.id))
				{
					target.target = tag.id;
					changed = true;
				}
			}
			if (ImGui::Selectable("None", target.target == 0))
			{
				target.target = 0;
				changed = true;
			}
			ImGui::EndCombo();
		}

		if(DrawVec3Control("Offset", glm::value_ptr(target.headOffset)))
			changed = true;

		ImGui::NewLine();

		return changed;
	}

	bool DrawSelectableInspector(EditorContext& eCtx, entt::registry& reg, SelectableComponent& selectable)
	{
		bool changed = false;

		ImGui::SeparatorText("Interactable");
		ImGui::Text("Interactable"); ImGui::SameLine();
		if (ImGui::Checkbox("##interactable", &selectable.interactable))
			changed = true;

		ImGui::SeparatorText("Normal Color");
		if(ImGui::ColorEdit4("##normalcolor", glm::value_ptr(selectable.normalColor)))
			changed = true;

		ImGui::SeparatorText("Highlighted Color");
		if(ImGui::ColorEdit4("##highlightedColor", glm::value_ptr(selectable.highlightedColor)))
			changed = true;

		ImGui::SeparatorText("Pressed Color");
		if(ImGui::ColorEdit4("##pressedColor", glm::value_ptr(selectable.pressedColor)))
			changed = true;

		ImGui::NewLine();

		return changed;
	}

	bool DrawParticleInspector(EditorContext& eCtx, entt::registry& reg, ParticleEmitterComponent& particle)
	{
		// Not check dirty for particle
		bool changed = false;
		auto& particleRender = reg.get<ParticleRenderComponent>(eCtx.selectedEntity);
		if (ImGui::BeginTable("ParticleInformation", 2))
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Duration");

			ImGui::TableSetColumnIndex(1);
			float duration = std::chrono::duration<float>(particle.emitter.duration).count();
			if (ImGui::InputFloat("##Duration", &duration, 0.f, 0.f, "%g"))
			{
				particle.emitter.duration =
					std::chrono::duration_cast<std::chrono::milliseconds>(
						std::chrono::duration<float>(duration)
					);
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Looping");

			ImGui::TableSetColumnIndex(1);
			ImGui::Checkbox("##Looping", &particle.looping);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Start Delay");

			ImGui::TableSetColumnIndex(1);
			float startDelay = std::chrono::duration<float>(particle.startDelay).count();
			if (ImGui::InputFloat("##StartDelay", &startDelay, 0.f, 0.f, "%g"))
			{
				particle.startDelay =
					std::chrono::duration_cast<std::chrono::milliseconds>(
						std::chrono::duration<float>(startDelay)
					);
			}
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Start Lifetime");

			ImGui::TableSetColumnIndex(1);
			float startLifetime = std::chrono::duration<float>(particle.particleLifetime).count();
			if (ImGui::InputFloat("##StartLifetime", &startLifetime, 0.f, 0.f, "%g"))
			{
				particle.particleLifetime =
					std::chrono::duration_cast<std::chrono::milliseconds>(
						std::chrono::duration<float>(startLifetime)
					);
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Start Speed");

			ImGui::TableSetColumnIndex(1);
			ImGui::InputFloat("##StartSpeed", &particle.startSpeed, 0.f, 0.f, "%g");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Start Size");

			ImGui::TableSetColumnIndex(1);
			ImGui::InputFloat("##StartSize", &particleRender.size, 0.f, 0.f, "%g");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Start Color");

			ImGui::TableSetColumnIndex(1);
			ImVec4 pColor = { particleRender.color.x, particleRender.color.y, particleRender.color.z, particleRender.color.w };
			if (ImGui::ColorButton("##start Color", pColor))
				ImGui::OpenPopup("picker");

			if (ImGui::BeginPopup("picker"))
			{
				ImGui::ColorPicker4("##colorEdit4", glm::value_ptr(particleRender.color));
				ImGui::EndPopup();
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Max Paritcles");

			ImGui::TableSetColumnIndex(1);
			ImGui::DragInt("##MaxParticles", &particle.maxParticles);
			
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Simulation Space");

			ImGui::TableSetColumnIndex(1);
			const char* spacePreview = particleSimulationSpaceMetas[(uint8_t)particle.space].name;
			if (ImGui::BeginCombo("##Space", spacePreview))
			{
				for (const auto& space : particleSimulationSpaceMetas)
				{
					if(ImGui::Selectable(space.name, particle.space == space.type))
					{
						particle.space = space.type;
					}
				}
				ImGui::EndCombo();
			}

			/*auto& runtimeParticle = reg.get<ParticleRuntimeComponent>(eCtx.selectedEntity);
			auto& targetParticle = reg.get<TargetComponent>(eCtx.selectedEntity);*/
			/*ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Custom");

			ImGui::TableSetColumnIndex(1);
			static bool bTarget = true;
			if (ImGui::Checkbox("##check", &bTarget))
				targetParticle.target = bTarget ? targetParticle.target : 0;

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Target");

			ImGui::TableSetColumnIndex(1);
			
			ImGui::BeginDisabled(!bTarget);
			entt::entity targetEntity = GetEntityByUUID(reg, targetParticle.target);
			const char* namePreview = targetEntity == entt::null ?
				"None" : reg.try_get<NametagComponent>(targetEntity)->name.c_str();
			
			if (ImGui::BeginCombo("##entityCombo", namePreview))
			{
				auto view = reg.view<NametagComponent, TransformComponent>();
				for (auto [e, tag, transform] : view.each())
				{
					if (ImGui::Selectable(tag.name.c_str(), targetParticle.target == tag.id))
						targetParticle.target = tag.id;
				}
				ImGui::EndCombo();
			}
			ImGui::EndDisabled();*/

			ImGui::EndTable();
		}
		
		ImGui::Separator();
		if (ImGui::BeginTable("Shape", 2))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Shape");

			ImGui::TableSetColumnIndex(1);
			const char* shapePreview = shapeMetas[(uint8_t)particle.shape].name;
			if (ImGui::BeginCombo("##Shape", shapePreview))
			{
				for (const auto& shape : shapeMetas)
				{
					if (ImGui::Selectable(shape.name, particle.shape == shape.shape))
					{
						particle.shape = shape.shape;
					}
				}
				ImGui::EndCombo();
			}

			if (particle.shape == ParticleEffectShape::Cone)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Angle");

				ImGui::TableSetColumnIndex(1);
				ImGui::InputFloat("##coneAngle", &particle.angle, 0.f, 0.f, "%g");
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Texture");

			ImGui::TableSetColumnIndex(1);
			std::filesystem::path curTex = AssetRegistry<Texture>::GetInstance().GetName(particleRender.texture);
			if (ImGui::BeginCombo("##texture", curTex.filename().string().c_str()))
			{
				auto it = AssetRegistry<Texture>::GetInstance().GetNameMapBegin();
				auto endIt = AssetRegistry<Texture>::GetInstance().GetNameMapEnd();
				for (it; it != endIt; it++)
				{
					curTex = it->second;
					if (ImGui::Selectable(curTex.filename().string().c_str(), particleRender.texture == it->first))
					{
						particleRender.texture = it->first;
						changed = true;
					}
				}
				ImGui::EndCombo();
			}

			auto loadSrc = DrawLoadResourceButton("Open File", "All Images \0*.png;*.jpg;*.jpeg\0\0", PathManager::ProjectResource() / "Img");
			if (!loadSrc.empty())
				particleRender.texture = GetAssetID(loadSrc);

			ImGui::EndTable();
		}

		ImGui::Separator();
		if (ImGui::BeginTable("Emission", 2))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Rate over Time");

			ImGui::TableSetColumnIndex(1);
			float rateOverTime = particle.emitPeriod == std::chrono::milliseconds::zero() ?
				0.f : 1.f / std::chrono::duration<float>(particle.emitPeriod).count();
			
			if (ImGui::InputFloat("##RateOverTime", &rateOverTime, 0.f, 0.f, "%g"))
			{
				particle.emitPeriod = rateOverTime == 0.f ?
					std::chrono::milliseconds::zero() :
					std::chrono::duration_cast<std::chrono::milliseconds>(
						std::chrono::duration<float>(1.f / rateOverTime)
					);
			}
			ImGui::EndTable();
		}

		ImGui::Text("Bursts");
		ImGui::SameLine();
		if(ImGui::Button("+", ImVec2(15.f, 15.f)))
		{
			if (!particle.burst.has_value())
				particle.burst.emplace(
					std::chrono::milliseconds::zero(),
					std::chrono::milliseconds::zero(),
					std::chrono::steady_clock::now(), 1, 0, 5);
		}

		ImGui::SameLine();
		if (ImGui::Button("-", ImVec2(15.f, 15.f)))
		{
			if (particle.burst.has_value())
				particle.burst.reset();
		}

		if (ImGui::BeginTable("Burst", 3, ImGuiTableFlags_SizingStretchSame))
		{
				ImGui::TableSetupColumn("Period", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Count", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Cycles", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableHeadersRow();

			if(particle.burst.has_value())
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				float burstPeriod = particle.burst.value().period == std::chrono::milliseconds::zero() ?
					0.f : 1.f / std::chrono::duration<float>(particle.burst.value().period).count();

				if (ImGui::InputFloat("##burstPeriod", &burstPeriod, 0.f, 0.f, "%g"))
				{
					particle.burst.value().period =
						std::chrono::duration_cast<std::chrono::milliseconds>(
							std::chrono::duration<float>(burstPeriod)
						);
				}

				ImGui::TableSetColumnIndex(1);
				int burstCount = (int)particle.burst.value().count;
				if (ImGui::DragInt("##count", &burstCount, 1.f, 0, MAX_PARTICLE_NUM, "%d", ImGuiSliderFlags_AlwaysClamp))
				{
					particle.burst.value().count = burstCount;
					if (particle.maxParticles < burstCount)
					{
						int newSize = std::min(MAX_PARTICLE_NUM, burstCount);
						auto& buffer = reg.get<ParticleBufferComponent>(eCtx.selectedEntity);
						buffer.positions.resize(newSize + 1);
						buffer.velocities.resize(newSize + 1);
						buffer.lifetimes.resize(newSize + 1);
					}
				}
				ImGui::TableSetColumnIndex(2);
				ImGui::DragInt("##cycles", &particle.burst.value().cycles);
			}
				ImGui::EndTable();
		}

		ImGui::NewLine();

		return changed;
	}

	std::filesystem::path DrawLoadResourceButton(const char* title, const char* filter, const std::filesystem::path& initDir)
	{
		ImGui::SameLine();

		// Loads files by converting the project path (source path)
		// to the runtime path and copying the file.
		//
		// @Note:
		// The file must be located under project path/Contents/Resources.
		if (ImGui::Button("+"))
		{
			auto path = FileDialog::OpenFile(title, filter, initDir);

			if (path.has_value())
			{
				auto extension = path.value().extension();
				auto runtimePath = PathManager::ToRuntime(path.value());
				
				FileUtils::CopyAsset(path.value(), runtimePath);

				if (extension == ".png" || extension == ".jpg" || extension == ".jpeg")
					Texture::Create(runtimePath);

				else if (extension == ".ptc")
					ParticleEffect::Create(runtimePath);

				else if (extension == ".ttf")
					Font::Create(runtimePath);

				else if (extension == ".mp3")
					Audio::Create(runtimePath);

				else
				{
					TMT_ERR << "Invalid file extension" << extension;
					return "";
				}

				return runtimePath;
			}
		}

		return "";
	}

	bool DrawVec3Control(const char* label, float* vec, float min, float max, int flags)
	{
		ImGui::SeparatorText(label);

		float avail = ImGui::GetContentRegionAvail().x;
		float firstPos = avail / 3.f / 2.f;
		float textSize = ImGui::CalcTextSize("X").x;

		ImGui::SetCursorPosX(firstPos + textSize / 2.f);
		ImGui::Text("X"); ImGui::SameLine();

		textSize = ImGui::CalcTextSize("Y").x;
		ImGui::SetCursorPosX(firstPos += avail / 3.f + textSize / 2.f);
		ImGui::Text("Y"); ImGui::SameLine();

		textSize = ImGui::CalcTextSize("Z").x;
		ImGui::SetCursorPosX(firstPos += avail / 3.f + textSize / 2.f);
		ImGui::Text("Z");

		ImGui::PushItemWidth(avail);
		
		std::string tmpLabel = "##";
		tmpLabel += label;

		float v_min = min == 0 ? std::numeric_limits<float>::lowest() : min;
		float v_max = max == 0 ? std::numeric_limits<float>::max() : max;
		if (ImGui::DragFloat3(tmpLabel.c_str(), vec, 1.f, v_min, v_max, "%g", flags))
			return true;

		ImGui::PopItemWidth();

		return false;
	}
}


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

#include "State/State.h"
#include "ECS/Components/Components.h"
#include "ECS/Entity/Hierarchy.h"
#include "ECS/Entity/Entity.h"

#include "Particle/ParticleConfig.h"

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

	bool DrawInputChannelInspcetor(EditorContext& eCtx, entt::registry& reg, InputChannelComponent& inputCh)
	{
		bool changed = false;

		ImGui::SeparatorText("Key Setting");
		if (ImGui::RadioButton("WASD", inputCh.useWASD == true))
		{
			inputCh.useWASD = true;
			changed = true;
		}

		ImGui::SameLine();

		if (ImGui::RadioButton("Number Keys", inputCh.useWASD == false))
		{
			inputCh.useWASD = false;
			changed = true;
		}

		ImGui::NewLine();

		return changed;
	}

	bool DrawTransformInspector(EditorContext& eCtx, entt::registry& reg, TransformComponent& transform)
	{
		bool changed = false;

		glm::vec3 pos = transform.GetLocalPosition();
		float posVec3[3] = { pos.x, pos.y, pos.z };
		if (DrawVec3Control("Position", posVec3, 0.f, 0.f, ImGuiSliderFlags_ColorMarkers))
		{
			transform.SetPosition(posVec3[0], posVec3[1], posVec3[2]);
			changed = true;
		}

		glm::vec3 degree = transform.GetLocalRotationDegree();
		float degreeVec3[3] = { degree.x, degree.y, degree.z };
		if (DrawVec3Control("Degree", degreeVec3, 0.f, 0.f, ImGuiSliderFlags_ColorMarkers))
		{
			transform.SetRotationDegree(degreeVec3[0], degreeVec3[1], degreeVec3[2]);
			changed = true;
		}

		glm::vec3 scale = transform.GetLocalScale();
		float scaleVec3[3] = { scale.x, scale.y, scale.z };
		if (DrawVec3Control("Scale", scaleVec3, 1e-4f, 0.f, ImGuiSliderFlags_ClampOnInput | ImGuiSliderFlags_ColorMarkers))
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
		if(ImGui::Checkbox("##Trigger", &collider.trigger))
			changed = true;

		ImGui::SeparatorText("Layer");
		const char* layPreview = "Default";
		for (const auto& info : CollisionLayerMetas)
		{
			if (collider.layer == info.layer)
			{
				layPreview = info.name;
				break;
			}
		}

		if (ImGui::BeginCombo("##Layer", layPreview))
		{
			for (const auto& info : CollisionLayerMetas)
			{
				if (ImGui::Selectable(info.name, collider.layer == info.layer))
				{
					collider.layer = info.layer;
					changed = true;
				}
			}

			ImGui::EndCombo();
		}
		ImGui::NewLine();

		//Layer Matrix
		auto& matrix = eCtx.currentState->GetCollisionLayerMatrix();

		const int layerSize = static_cast<int>(CollisionLayer::COUNT);
		ImGuiTableColumnFlags column_flags = ImGuiTableColumnFlags_AngledHeader | ImGuiTableColumnFlags_WidthFixed;
		if (ImGui::BeginTable("Layer Collision Matrix", layerSize + 1))
		{
			ImGui::TableSetupColumn("CollisionLayer", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_WidthFixed);
			
			for (int i = 0; i < layerSize; ++i) 
				ImGui::TableSetupColumn(CollisionLayerMetas[i].name, column_flags | ImGuiTableColumnFlags_NoHeaderWidth);

			ImGui::TableAngledHeadersRow();

			for (int i = 0; i < layerSize; ++i)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				ImGui::AlignTextToFramePadding();
				ImGui::Text(CollisionLayerMetas[i].name);

				for (int j = layerSize - 1; j >= i; --j)
				{
					ImGui::TableSetColumnIndex(j+1);

					const auto layerA = CollisionLayerMetas[i].layer;
					const auto layerB = CollisionLayerMetas[j].layer;

					bool canCollide = matrix.CanCollide(layerA, layerB);

					ImGui::PushID(i * layerSize + j);

					if (ImGui::Checkbox("##Collsion", &canCollide))
						matrix.SetCollisionLayer(layerA, layerB, canCollide);
					
					ImGui::PopID();
				}
				ImGui::NewLine();
			}
			ImGui::EndTable();
		}
		
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

		auto itBegin = AssetRegistry<Mesh>::GetInstance().GetNameMapBegin();
		auto itEnd = AssetRegistry<Mesh>::GetInstance().GetNameMapEnd();

		if (ImGui::BeginCombo("##mesh", meshPreview))
		{
			for (itBegin; itBegin != itEnd; ++itBegin)
			{
				if (ImGui::Selectable(
					itBegin->second.c_str() + 11,
					render.mesh == itBegin->first))
				{
					render.mesh = itBegin->first;
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

	bool DrawHierarchyInspector(EditorContext& eCtx, entt::registry& reg, HierarchyComponent& hier)
	{
		ImGui::SeparatorText("root");
		ImGui::Text(std::to_string(entt::to_integral(hier.root)).c_str());
		
		ImGui::SeparatorText("parent");
		if(hier.parent == entt::null)
			ImGui::Text("entt::null");
		else
			ImGui::Text(std::to_string(entt::to_integral(hier.parent)).c_str());

		ImGui::SeparatorText("children");
		for (auto child : hier.children)
			ImGui::Text(std::to_string(entt::to_integral(child)).c_str());

		return true;
	}

	bool DrawUIInspector(EditorContext& eCtx, entt::registry& reg, UIComponent& ui)
	{
		bool changed = false;

		if (ui.canvas != entt::null)
		{
			ImGui::SeparatorText("Canvas");
			auto canvasView = reg.view<CanvasComponent, NametagComponent>();
			auto curCanvas = reg.try_get<NametagComponent>(ui.canvas);
			const char* canvasPreview = curCanvas != nullptr ? curCanvas->name.c_str() : "null";
			if (ImGui::BeginCombo("##canvas", canvasPreview))
			{
				for (auto [e, canvas, tag] : canvasView.each())
				{
					if (ImGui::Selectable(tag.name.c_str(), ui.canvas == e))
					{
						ui.canvas = e;
						SetHierarchy(reg, ui.canvas, eCtx.selectedEntity);
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
		const char* modePreview = UIRenderModeMetas[(int)canvas.mode].name;
		if (ImGui::BeginCombo("##renderMode", modePreview))
		{
			for (auto meta : UIRenderModeMetas)
			{
				//@TODO: Not support ScreenCamera, Normal World UI
				if (ImGui::Selectable(meta.name, canvas.mode == meta.mode))
				{
					canvas.mode = meta.mode;
					changed = true;
				}
			}
			ImGui::EndCombo();
		}
		
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
			glm::vec2 pos = rect.anchoredPosition;

			ImGui::SeparatorText("Position");
			ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(60.f);
			if (ImGui::DragFloat("##anchoredPosx", &pos.x, 1.f, 0.f, 0.f, "%g"))
			{
				rect.SetPosition(pos);
				changed = true; 
			}

			ImGui::SameLine();
			ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(60.f);
			if (ImGui::DragFloat("##anchoredPosy", &pos.y, 1.f, 0.f, 0.f, "%g"))
			{
				rect.SetPosition(pos);
				changed = true;
			}

			if (ui && ui->type != UIType::Text)
			{
				glm::vec2 size = rect.sizeDelta;

				ImGui::SeparatorText("Size");
				ImGui::Text("Width"); ImGui::SameLine(); ImGui::SetCursorPosX(95.f);
				ImGui::Text("Height"); ImGui::SetNextItemWidth(80.f);
				if (ImGui::DragFloat("##sizewidth", &size.x, 1.f, 0.f, 0.f, "%g"))
				{
					rect.SetSize(size);
					changed = true;
				}

				ImGui::SameLine(); ImGui::SetNextItemWidth(80.f);
				if (ImGui::DragFloat("##sizeheight", &size.y, 1.f, 0.f, 0.f, "%g"))
				{
					rect.SetSize(size);
					changed = true;
				}
			}

			/*ImGui::SeparatorText("Offset");
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
				changed = true;*/
		}
		else
		{
			glm::vec2 offsetMin{ rect.offsetMin.x, rect.offsetMin.y };
			float offsetRight{ std::abs(rect.offsetMax.x) }, offsetTop{ std::abs(rect.offsetMax.y) };

			ImGui::SeparatorText("Margin"); // Offset? Margin?
			ImGui::Text("Left");
			ImGui::SameLine(); ImGui::SetCursorPosX(95.f);
			ImGui::Text("Top");

			ImGui::SetNextItemWidth(80.f);
			if (ImGui::InputFloat("##Left", &offsetMin.x, 0.0f, 0.0f, "%g"))
			{
				rect.SetOffsetMin(offsetMin);
				changed = true; 
			}
			
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.f);
			if (ImGui::InputFloat("##Top", &offsetTop, 0.0f, 0.0f, "%g"))
			{
				rect.SetOffsetMax({ rect.offsetMax.x, -offsetTop });
				//rect.offsetMax.y = -offsetTop;
				changed = true;
			}

			ImGui::Text("Right");
			ImGui::SameLine(); ImGui::SetCursorPosX(95.f);
			ImGui::Text("Bottom");
			ImGui::SetNextItemWidth(80.f);

			if(ImGui::InputFloat("##Right", &offsetRight, 0.0f, 0.0f, "%g"))
			{
				rect.SetOffsetMax({ -offsetRight, rect.offsetMax.y });
				//rect.offsetMax.x = -offsetRight;
				changed = true;
			}
			ImGui::SameLine(); ImGui::SetNextItemWidth(80.f);
			if (ImGui::InputFloat("##Bottom", &offsetMin.y, 0.0f, 0.0f, "%g"))
			{
				rect.SetOffsetMin(offsetMin);
				changed = true;
			}
		}

		glm::vec2 anchorMin{ rect.anchorMin }, anchorMax{ rect.anchorMax };
		ImGui::SeparatorText("Anchor");
		ImGui::Text("Min"); ImGui::SameLine(); ImGui::SetCursorPosX(50.f); ImGui::SameLine();
		ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.f);
		if(ImGui::DragFloat("##anchorMinX", &anchorMin.x, 0.1f, 0.f, 1.f, "%.1f"))
		{
			rect.SetAnchor(anchorMin, anchorMax);
			changed = true; 
		}
		
		ImGui::SameLine();
		ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.f);
		if (ImGui::DragFloat("##anchorMinY", &anchorMin.y, 0.1f, 0.f, 1.f, "%.1f"))
		{
			rect.SetAnchor(anchorMin, anchorMax);
			changed = true;
		}

		ImGui::Text("Max");ImGui::SameLine(); ImGui::SetCursorPosX(40.f); ImGui::SameLine();
		ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.f);
		if (ImGui::DragFloat("##anchorMaxX", &anchorMax.x, 0.1f, 0.f, 1.f, "%.1f"))
		{
			rect.SetAnchor(anchorMin, anchorMax);
			changed = true; 
		}
		
		ImGui::SameLine();
		ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.f);
		if (ImGui::DragFloat("##anchorMaxY", &anchorMax.y, 0.1f, 0.f, 1.f, "%.1f"))
		{
			rect.SetAnchor(anchorMin, anchorMax);
			changed = true;
		}

		glm::vec2 pivot{ rect.pivot };
		ImGui::SeparatorText("Pivot");
		ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(60.f);
		if (ImGui::DragFloat("##pivotx", &pivot.x, 0.1f, 0.f, 1.f, "%.1f"))
		{
			rect.SetPivot(pivot);
			changed = true; 
		}
		
		ImGui::SameLine();
		ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(60.f);
		if (ImGui::DragFloat("##pivoty", &pivot.y, 0.1f, 0.f, 1.f, "%.1f"))
		{
			rect.SetPivot(pivot);
			changed = true;
		}

		ImGui::NewLine();

		return changed;
	}

	bool DrawTextInspector(EditorContext& eCtx, entt::registry& reg, TextComponent& text)
	{
		bool changed = false;

		ImGui::SeparatorText("Text");
		if (ImGui::InputText("##text", &text.text, ImGuiInputTextFlags_None))
		{
			text.dirty = true;
			changed = true;
		}

		ImGui::SeparatorText("Color");
		if(ImGui::ColorEdit4("##textcolor", glm::value_ptr(text.color)))
			changed = true;

		ImGui::SeparatorText("Size");
		if(ImGui::DragFloat("##fontSize", &text.fontSize, 1.f, 1e-4f, 100.0f, "%g", ImGuiSliderFlags_AlwaysClamp))
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
		
		entt::entity targetEntity = target.target;
		const char* namePreview = targetEntity == entt::null ? "None" : reg.try_get<NametagComponent>(targetEntity)->name.c_str();
		if (ImGui::BeginCombo("##entityCombo", namePreview))
		{
			auto view = reg.view<NametagComponent, TransformComponent>();
			for (auto [e, tag, transform] : view.each())
			{
				if (ImGui::Selectable(tag.name.c_str(), target.target == e))
				{
					target.target = e;
					changed = true;
				}
			}
			if (ImGui::Selectable("None", target.target == entt::null))
			{
				target.target = entt::null;
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

	bool DrawParticleInspector(EditorContext& eCtx, entt::registry& reg, ParticleEmitterComponent& emitter)
	{
		bool changed = false;

		auto& sets = emitter.sets;
		const char* preview;
		if (ImGui::BeginTable("Main emitter", 2))
		{
			// Duration
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Duration");

			ImGui::TableSetColumnIndex(1);
			if (ImGui::InputFloat("##Duration", &sets.duration))
				changed = true;

			// Looping
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Looping");

			ImGui::TableSetColumnIndex(1);
			if (ImGui::Checkbox("##Looping", &sets.looping))
				changed = true;

			// Start delay
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Start Delay");

			ImGui::TableSetColumnIndex(1);
			if (ImGui::InputFloat("##StartDelay", &sets.startDelay))
				changed = true;

			// Max particles
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Max Particles");

			ImGui::TableSetColumnIndex(1);
			int maxParticles = sets.maxParticles;
			if (ImGui::InputInt("##MaxParticles", &maxParticles))
			{
				sets.maxParticles = std::clamp(maxParticles,
					0, static_cast<int>(reg.ctx().get<ParticleConfig>().maxParticlesPerEmitter));
				changed = true;
			}

			// Simulation space
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Simulation Space");

			ImGui::TableSetColumnIndex(1);
			preview = "World";
			for (const auto& info : particleSimulationSpaceMetas)
			{
				if (sets.simSpace == info.type)
				{
					preview = info.name;
					break;
				}
			}

			if (ImGui::BeginCombo("##SimulationSpace", preview))
			{
				for (const auto& info : particleSimulationSpaceMetas)
				{
					if (ImGui::Selectable(info.name, sets.simSpace == info.type))
					{
						sets.simSpace = info.type;
						changed = true;
					}
				}

				ImGui::EndCombo();
			}
		}
		ImGui::EndTable();
		ImGui::Separator();

		if (ImGui::BeginTable("Main particles", 2))
		{
			// Lifetime
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Lifetime");

			ImGui::TableSetColumnIndex(1);
			if (ImGui::InputFloat("##Lifetime", &sets.lifetime))
				changed = true;

			// Speed
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Speed");

			ImGui::TableSetColumnIndex(1);
			if (ImGui::InputFloat("##Speed", &sets.speed))
				changed = true;

			// Size
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Size");

			ImGui::TableSetColumnIndex(1);
			if (ImGui::InputFloat("##Size", &sets.size))
				changed = true;

			// Color
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Color");

			ImGui::TableSetColumnIndex(1);
			if (ImGui::ColorEdit4("##Color", glm::value_ptr(sets.color)))
				changed = true;
		}
		ImGui::EndTable();
		ImGui::Separator();

		if (ImGui::BeginTable("Shape", 2))
		{
			// Emitter shape
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Emitter Shape");

			ImGui::TableSetColumnIndex(1);
			preview = "Cone";
			for (const auto& info : particleEmitterShapeMetas)
			{
				if (sets.shape == info.shape)
				{
					preview = info.name;
					break;
				}
			}

			if (ImGui::BeginCombo("##EmitterShape", preview))
			{
				for (const auto& info : particleEmitterShapeMetas)
				{
					if (ImGui::Selectable(info.name, sets.shape == info.shape))
					{
						sets.shape = info.shape;
						changed = true;
					}
				}

				ImGui::EndCombo();
			}

			// Cone angle
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Cone Angle");

			ImGui::TableSetColumnIndex(1);
			if (ImGui::InputFloat("##ConeAngle", &sets.coneAngle))
				changed = true;
		}
		ImGui::EndTable();
		ImGui::Separator();

		if (ImGui::BeginTable("Render", 2))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Texture");

			ImGui::TableSetColumnIndex(1);
			std::filesystem::path curTex;
			if (sets.texture != 0)
				curTex = AssetRegistry<Texture>::GetInstance().GetName(sets.texture);
			else
				curTex = AssetRegistry<Texture>::GetInstance().GetName(GetAssetID(Texture::PrimitiveName));
			if (ImGui::BeginCombo("##texture", curTex.filename().string().c_str()))
			{
				auto it = AssetRegistry<Texture>::GetInstance().GetNameMapBegin();
				auto endIt = AssetRegistry<Texture>::GetInstance().GetNameMapEnd();
				for (it; it != endIt; ++it)
				{
					curTex = it->second;
					if (ImGui::Selectable(curTex.filename().string().c_str(), sets.texture == it->first))
					{
						sets.texture = it->first;
						changed = true;
					}
				}
				ImGui::EndCombo();
			}
		}
		ImGui::EndTable();
		ImGui::Separator();

		if (ImGui::BeginTable("Emission", 2))
		{
			// Rate over time
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Rate Over Time");

			ImGui::TableSetColumnIndex(1);
			if (ImGui::InputFloat("##RateOverTime", &sets.rateOverTime))
				changed = true;
		}
		ImGui::EndTable();

		// Bursts
		ImGui::Text("Bursts");
		ImGui::SameLine();
		if (ImGui::Button("+", ImVec2(15.0f, 15.0f)))
		{
			sets.bursts.emplace_back();
		}

		ImGui::SameLine();
		if (ImGui::Button("-", ImVec2(15.0f, 15.0f)))
		{
			sets.bursts.pop_back();
		}

		if (ImGui::BeginTable("Bursts", 4, ImGuiTableFlags_SizingStretchSame))
		{
			ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Count", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Cycles", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Interval", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow();

			for (auto& burst : sets.bursts)
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				if (ImGui::InputFloat("##Time", &burst.time))
					changed = true;

				ImGui::TableSetColumnIndex(1);
				int cnt = burst.count;
				if (ImGui::InputInt("##Count", &cnt))
				{
					burst.count = std::max(0, cnt);
					changed = true;
				}

				ImGui::TableSetColumnIndex(2);
				int cycles = burst.cycles;
				if (ImGui::InputInt("##Cycles", &cnt))
				{
					burst.cycles = std::max(0, cycles);
					changed = true;
				}

				ImGui::TableSetColumnIndex(3);
				if (ImGui::InputFloat("##Interval", &burst.interval))
					changed = true;
			}
		}
		ImGui::EndTable();

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


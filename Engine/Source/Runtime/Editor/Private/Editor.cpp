#include "Editor.h"

#include <entt/entt.hpp>
#include "Resource/PathManager.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Texture.h"

#include "ECS/Components/Render.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/UI.h"
#include "ECS/Components/Visibility.h"
#include "ECS/Components/Hierarchy.h"

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <implot/implot.h>

#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "Services/Input.h"
#include "Services/Window.h"
#include "State/State.h"

#include "Input/KeyDeviceState.h"

#include "EditorPanel.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"

#include "Math/Geometry.h"

namespace tomato
{
	void Editor::InitImGui(GLFWwindow* wnd, Input& input)
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

		// Setup ImPlot context
		ImPlot::CreateContext();
		
		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(wnd, false);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
		ImGui_ImplOpenGL3_Init();
		
		SetInputCallbacks(input);

		ImFontConfig config;

		config.MergeMode = true;
		config.PixelSnapH = true;
		config.OversampleH = 1;
		config.OversampleV = 1;

		io.Fonts->AddFontDefault();

		io.Fonts->AddFontFromFileTTF(
			PathManager::RuntimeFont("Pretendard-SemiBold.ttf").string().c_str(),
			0,
			&config,
			io.Fonts->GetGlyphRangesKorean());

		LoadResources();

		eCtx.selectedEntity = entt::null;
		panels.push_back(std::make_unique<HierarchyPanel>(400.f, 300.f, Window::GetWidth(), 320.f));
		panels.push_back(std::make_unique<InspectorPanel>(300.f, 600.f, Window::GetWidth(), 900.f));
	}

	void Editor::ShutdownImGui(Input& input)
	{
        input.GetMouseButtonSink().disconnect<ImGui_ImplGlfw_MouseButtonCallback>();
        input.GetCursorPosSink().disconnect<ImGui_ImplGlfw_CursorPosCallback>();
        input.GetScrollSink().disconnect<ImGui_ImplGlfw_ScrollCallback>();
        input.GetKeySink().disconnect<ImGui_ImplGlfw_KeyCallback>();
        input.GetCharSink().disconnect<ImGui_ImplGlfw_CharCallback>();

		ImPlot::DestroyContext();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void Editor::BeginFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void Editor::Draw(State* state, RunMode& mode)
	{
		//ImGui::ShowDemoWindow();
		if (eCtx.currentState != state)
		{
			eCtx.currentState = state;
			ResetEditorContext(state);
		}

		mainMenu_.Draw(eCtx, mode);
		for (auto& panel : panels)
			panel->Draw(eCtx);
		
		if (KeyDeviceState::GetInstance().IsKeyPressed(Key::LeftMouseButton))
		{
			auto& keyDeviceState = KeyDeviceState::GetInstance();
			PickObject(eCtx.currentState->GetRegistry(),
				{keyDeviceState.GetKeyState(Key::MouseX), keyDeviceState.GetKeyState(Key::MouseY)});
		}
	}

	void Editor::EndFrame()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void Editor::SetInputCallbacks(Input& input)
	{
		input.GetMouseButtonSink().connect<ImGui_ImplGlfw_MouseButtonCallback>();
		input.GetCursorPosSink().connect<ImGui_ImplGlfw_CursorPosCallback>();
		input.GetScrollSink().connect<ImGui_ImplGlfw_ScrollCallback>();
		input.GetKeySink().connect<ImGui_ImplGlfw_KeyCallback>();
		input.GetCharSink().connect<ImGui_ImplGlfw_CharCallback>();
	}

	void Editor::PickObject(entt::registry& reg, glm::vec2 mousePos)
	{
		// 1. Check the first click position and exclude ImGui window areas
		// Note: Calculated using a top-left coordinate system and assuming the pivot is (1, 1)
		if (IsMouseOverPanel(mousePos))
			return;

		// 2-0 Check UI
		if (TrySelectUI(reg, glm::vec2(mousePos.x, Window::GetHeight() - mousePos.y)))
			return;

		Ray ray = ScreenPointToRay(reg, mousePos);

		float nearest = FLT_MAX;
		entt::entity selected = entt::null;

		auto view = reg.view<VisibilityComponent, TransformComponent, RenderComponent, RootEntityTag>();
		for (auto [e, visib, transform, render] : view.each())
		{
			if (!visib.visible) continue;

			auto mesh = AssetRegistry<Mesh>::GetInstance().Get(render.mesh);
			AABB worldAABB = TransformAABB(mesh->GetLocalAABB(), transform.GetTransformMatrix());

			float distance;
			if (IntersectRayAABB(ray, worldAABB, distance))
			{
				if (distance < nearest)
				{
					nearest = distance;
					selected = e;
				}
			}
		}

		eCtx.selectedEntity = selected;
	}

	void Editor::LoadResources()
	{
		Texture::Create(PathManager::RuntimeIcon("visibility_on.png"));
		Texture::Create(PathManager::RuntimeIcon("visibility_off.png"));
		Texture::Create(PathManager::RuntimeIcon("more_vert.png"));
	}

	void Editor::ResetEditorContext(State* newState)
	{
		//eCtx.currentScenePath = "";
		eCtx.currentState = newState;
		eCtx.sceneDirty = false;
		eCtx.selectedEntity = entt::null;
	}

	bool Editor::IsMouseOverPanel(glm::vec2 mousePos)
	{
		for (auto& panel : panels)
		{
			glm::vec2 panelTopLeft = panel.get()->GetPos();
			glm::vec2 panelBotRight = panelTopLeft + panel.get()->GetSize();

			if (mousePos.x < panelTopLeft.x || mousePos.x > panelBotRight.x ||
				mousePos.y < panelTopLeft.y || mousePos.y > panelBotRight.y)
				continue;

			return true;
		}
		return false;
	}

	bool Editor::TrySelectUI(entt::registry& reg, glm::vec2 mousePos)
	{
		auto& uiCtx = reg.ctx().get<UIContext>();

		if (!uiCtx.screenDrawList.empty())
		{
			for (auto entity : uiCtx.screenDrawList)
			{
				auto& rect = reg.get<RectTransformComponent>(entity);

				if (mousePos.x >= rect.screenRect.min.x && mousePos.x <= rect.screenRect.max.x &&
					mousePos.y >= rect.screenRect.min.y && mousePos.y <= rect.screenRect.max.y)
				{
					eCtx.selectedEntity = entity;
					return true;
				}
			}
		}

		return false;
	}
}
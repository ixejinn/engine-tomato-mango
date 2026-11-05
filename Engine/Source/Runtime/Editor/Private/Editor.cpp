#include "Editor.h"
#include "Engine/Engine.h"

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
#include "Services/Window.h"
#include "State/State.h"
#include "Input/KeyDeviceState.h"

#include "EditorPanel.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"

#include "Math/Geometry.h"

namespace tomato
{
    bool Editor::OnKeyEvent(const KeyEvent&)
    {
        return !ImGui::GetIO().WantCaptureKeyboard;
    }

    bool Editor::OnMouseButtonEvent(const MouseButtonEvent&)
    {
        return !ImGui::GetIO().WantCaptureMouse;
    }

	void Editor::InitImGui(GLFWwindow* wnd, Input& input, Engine* engine)
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // 키보드 입력으로 ImGui의 포커스 이동을 막기 위해 주석 처리

		// Setup ImPlot context
		ImPlot::CreateContext();
		
		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(wnd, true);
		ImGui_ImplOpenGL3_Init();

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

		eCtx.selectedEntity = entt::null;
		eCtx.engine_ = engine;
		LoadResources();
		panels.push_back(std::make_unique<HierarchyPanel>(300.f, 300.f, Window::GetWidth(), 0.f));
		panels.push_back(std::make_unique<InspectorPanel>(300.f, 600.f, Window::GetWidth(), 300.f));
	}

	void Editor::ShutdownImGui(Input& input)
	{
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

	void Editor::Draw(uint64_t tick, State* state, RunMode& mode)
	{
		// ImGui::ShowDemoWindow();
        eCtx.tick = tick;
		if (eCtx.currentState != state)
		{
			eCtx.currentState = state;
			ResetEditorContext(state);
		}

		if (eCtx.selectedEntity != entt::null)
			panels[1]->open_ = true;

		mainMenu_.Draw(eCtx, mode);
		for (auto& panel : panels)
			panel->Draw(eCtx);
		
		if (KeyDeviceState::GetInstance().IsKeyPressed(Key::LeftMouseButton))
		{
			auto& keyDeviceState = KeyDeviceState::GetInstance();

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::GetIO().WantCaptureMouse)
				PickObject(eCtx.currentState->GetRegistry(),
					{keyDeviceState.GetKeyState(Key::MouseX), keyDeviceState.GetKeyState(Key::MouseY)});
		}
	}

	void Editor::EndFrame()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void Editor::PickObject(entt::registry& reg, glm::vec2 mousePos)
	{
		// Check UI
		if (TrySelectUI(reg, glm::vec2(mousePos.x, Window::GetHeight() - mousePos.y)))
			return;

		Ray ray = ScreenPointToRay(reg, mousePos);

		float nearest = FLT_MAX;
		entt::entity selected = eCtx.selectedEntity;

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
					panels[1]->open_ = true; // idx 1: inspector Panel
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
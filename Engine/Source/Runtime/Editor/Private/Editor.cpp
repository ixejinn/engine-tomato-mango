#include "Editor.h"

#include <entt/entt.hpp>
#include "Resource/Render/Texture.h"

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Services/Input.h"

#include "EditorPanel.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"

namespace tomato
{
	void Editor::InitImGui(GLFWwindow* wnd)
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		
		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(wnd, false);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
		ImGui_ImplOpenGL3_Init();
		
		SetInputCallbacks();

		ImFontConfig config;

		config.MergeMode = true;
		config.PixelSnapH = true;
		config.OversampleH = 1;
		config.OversampleV = 1;

		io.Fonts->AddFontDefault();

		io.Fonts->AddFontFromFileTTF(
			"Resources/Engine/Assets/Fonts/Pretendard-SemiBold.ttf",
			0,
			&config,
			io.Fonts->GetGlyphRangesKorean());

		LoadResources();

		eCtx.selectedEntity = entt::null;
		panels.push_back(std::make_unique<HierarchyPanel>(true));
		panels.push_back(std::make_unique<InspectorPanel>(true));
	}

	void Editor::LoadResources()
	{
		Texture::Create("Resources/Engine/Assets/img/visibility_on.png");
		Texture::Create("Resources/Engine/Assets/img/visibility_off.png");
	}

	void Editor::ShutdownImGui()
	{
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

	void Editor::Draw(State* state)
	{
#if 1
		ImGui::ShowDemoWindow();
		eCtx.currentState = state;
		for (auto& panel : panels)
			panel->Draw(eCtx);
		
#elif 1
#endif
	}

	void Editor::EndFrame()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void Editor::SetInputCallbacks()
	{
		InputCallbacks cb;
		cb.mouseButton = ImGui_ImplGlfw_MouseButtonCallback;
		cb.mouseMove = ImGui_ImplGlfw_CursorPosCallback;
		cb.scroll = ImGui_ImplGlfw_ScrollCallback;
		cb.key = ImGui_ImplGlfw_KeyCallback;
		cb.character = ImGui_ImplGlfw_CharCallback;

		Input::SetExternalInputCallbacks(cb);
	}
}
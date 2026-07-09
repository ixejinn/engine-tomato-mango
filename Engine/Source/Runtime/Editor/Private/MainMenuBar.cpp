#include "MainMenuBar.h"

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Serialization/ComponentSerializer.h"

namespace tomato
{
	void MainMenuBar::Draw()
	{
		if (ImGui::BeginMainMenuBar())
		{
			MenuFile();
			ImGui::EndMainMenuBar();
		}
	}

	void MainMenuBar::MenuFile()
	{
		if(ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New")) {}

			ImGui::EndMenu();
		}
	}
}
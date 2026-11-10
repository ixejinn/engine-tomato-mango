#include "NetworkPanel.h"

#include "Services/Window.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Event/EventDispatcher.h"
#include "Network/NetworkEvents.h"

namespace tomato
{
	NetworkPanel::NetworkPanel(float width, float height, float x, float y) : EditorPanel(width, height, x, y)
	{
		open_ = true;
	}

	void NetworkPanel::Draw(EditorContext& eCtx)
	{
		ImGui::SetNextWindowPos(ImVec2(posX_, ImGui::GetFrameHeight()), ImGuiCond_Once, ImVec2(0.f, 0.f));
		ImGui::SetNextWindowSize(ImVec2(width_, height_), ImGuiCond_Once);
		if (ImGui::Begin("Network", &open_, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			if (client == server)
				DrawNetworkModeButton();
			else if (client && !server)
				DrawClientModeButton();
		}
		ImGui::End();
	}
	void NetworkPanel::DrawNetworkModeButton()
	{
		if (ImGui::Button("Client", ImVec2(80, 40)))
		{
			client = true;
			EventDispatcher::GetInstance().Enqueue(Event::NetworkModeRequest{ client, false }); // { client, server }
		}
	}
	void NetworkPanel::DrawClientModeButton()
	{
		// @TODO : show address

		if (ImGui::Button("Stop Client", ImVec2(80, 40)))
		{
			client = false;
			EventDispatcher::GetInstance().Enqueue(Event::NetworkModeRequest{ client, false }); // { client, server }
		}
	}
}
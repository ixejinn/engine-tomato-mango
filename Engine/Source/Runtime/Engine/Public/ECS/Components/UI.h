#ifndef MANGO_UI_H
#define MANGO_UI_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <entt/entt.hpp>

namespace tomato {
	enum class RenderMode
	{
		ScreenOverlay,
		ScreenCamera,
		World
	};

	struct CanvasComponent // root
	{
		RenderMode mode{ RenderMode::ScreenOverlay };

		glm::vec2 referenceSize{ 1600.f, 900.f };
		glm::vec2 actualSize{ 1600.f, 900.f };

		entt::entity camera; // optinal
		int sortOrder{ 0 };
	};

	struct UIComponent
	{
		entt::entity canvas{ entt::null };
		int type{ 0 };
	}; // Tag

	struct UIRect
	{
		glm::vec2 min;
		glm::vec2 max;
	};

	struct RectTransformComponent
	{
		glm::vec2 anchoredPosition{ 0.f, 0.f }; // anchorMin == anchorMax, posX, posY

		glm::vec2 offsetMin{ 0.f, 0.f }; // stretch, left, bottom
		glm::vec2 offsetMax{ 0.f, 0.f }; //			 right, top

		glm::vec2 sizeDelta{ 1600.f, 900.f };

		glm::vec2 anchorMin{ 0.0f, 0.0f };
		glm::vec2 anchorMax{ 0.0f, 0.0f };

		glm::vec2 pivot{ 0.5f, 0.5f };

		// for calculation
		glm::vec2 computedSize{ 0.f, 0.f };
		glm::vec3 position{ 0.f, 0.f, 0.f }; //local position
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation{ 0.f, 0.f, 0.f };

		glm::mat4 local_matrix{ 1.f };
		glm::mat4 world_matrix{ 1.f };
		glm::mat4 model_matrix{ 1.f };

		// for hit test
		glm::vec3 screenPosition{ 0.f, 0.f, 0.f };
		UIRect screenRect;

		//bool dirty{ true };
	};

	struct HierarchyComponent
	{
		entt::entity parent{ entt::null };
		std::vector<entt::entity> children;
	};

	struct TargetComponent
	{
		entt::entity target{ entt::null };
		glm::vec3 headOffset{ 0.f, 100.f, 0.f };
	};

	inline void SetParent(entt::registry& registry, entt::entity child, entt::entity parent)
	{
		auto& childH = registry.get<HierarchyComponent>(child);
		if (childH.parent != entt::null)
		{
			auto& oldParentH = registry.get<HierarchyComponent>(childH.parent);
			auto& siblings = oldParentH.children;

			siblings.erase(std::remove(siblings.begin(), siblings.end(), child), siblings.end());
		}

		childH.parent = parent;
		if (parent != entt::null)
		{
			auto& newParentH = registry.get<HierarchyComponent>(parent);
			newParentH.children.push_back(child);
		}
	}

	//Separating "Selectable" related
	struct MouseEnterEvent
	{
		entt::entity e;
		entt::registry* reg;
	};

	using UICallBack = std::function<void(const MouseEnterEvent&)>;
	//inline void UICallTest(const MouseEnterEvent& event)
	//{
	//	/*std::cout << "MOUSE RELEASE\n";
	//	glfwSetWindowShouldClose(glfwGetCurrentContext(), true);*/
	//}
	struct SelectableComponent
	{
		bool interactable{ true };

		//transition
		glm::vec4 normalColor{ 0.2f, 0.75f, 0.4f, 1.0f };
		glm::vec4 highlightedColor{ 0.f, 1.f, 0.5f, 1.f };
		glm::vec4 pressedColor{ 1.f, 0.f, 1.f, 1.f };

		//Event
		UICallBack click; // = UICallTest;
	};
}

#endif //MANGO_UI_H
#ifndef MANGO_UISYSTEM_H
#define MANGO_UISYSTEM_H

#include <vector>
#include <glm/fwd.hpp>
#include <entt/fwd.hpp>
#include "ECS/Systems/System.h"
#include "ECS/Forward/SystemFwd.h"
#include "ECS/Forward/UIDirtyFwd.h"
namespace tomato
{
	class UISystem : public System
	{
	public:
		UISystem();
		void Update(SimContext& ctx) override;

	private:
		void OnChangeWindowSize();

		void Traverse(entt::registry& registry, entt::entity e, std::vector<entt::entity>& drawList, UI::Dirty dirty);
		void BuildDrawList(entt::registry& registry);
		void BulidSelectableList(entt::registry& registry);

		glm::vec3 WorldToScreen(
			const glm::vec3& worldPos,
			const glm::mat4& viewProjection,
			float screenWidth,
			float screenHeight);

		void UpdateTextContentSize(entt::registry& registry);
		void UpdateScreenRectTransform(entt::registry& registry);
		void UpdateWorldRectTransform(entt::registry& registry);

	private:
		SimContext* ctx{ nullptr };
	};

}

#endif // !MANGO_UISYSTEM_H

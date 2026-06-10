#ifndef MANGO_UISYSTEM_H
#define MANGO_UISYSTEM_H

#include <vector>
#include <glm/fwd.hpp>
#include <entt/fwd.hpp>
#include "ECS/Systems/System.h"
#include "ECS/SystemFwd.h"

namespace tomato
{
	class UISystem : public System
	{
	public:
		UISystem();
		void Update(SimContext& ctx) override;

	private:
		void Traverse(SimContext& ctx, entt::entity e, std::vector<entt::entity>& drawList);
		void BuildDrawList(SimContext& ctx);
		void BulidSelectableList(SimContext& ctx);

		glm::vec3 WorldToScreen(
			const glm::vec3& worldPos,
			const glm::mat4& viewProjection,
			float screenWidth,
			float screenHeight);

		void UpdateTextContentSize(SimContext& ctx);
		void UpdateRectTransform(SimContext& ctx);
	};

}

#endif // !MANGO_UISYSTEM_H

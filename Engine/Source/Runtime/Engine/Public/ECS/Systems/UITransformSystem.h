#ifndef MANGO_UITRANSFORMSYSTEM_H
#define MANGO_UITRANSFORMSYSTEM_H

#include <entt/fwd.hpp>
#include <glm/fwd.hpp>
#include "ECS/Systems/System.h"

namespace tomato
{
	class UITransformSystem : public System
	{
	public:
		void Update(SimContext& simCtx) override;

	private:
		void UpdateScreenUI(SimContext& simCtx);
	};
}
#endif // !MANGO_UITRANSFORMSYSTEM_H

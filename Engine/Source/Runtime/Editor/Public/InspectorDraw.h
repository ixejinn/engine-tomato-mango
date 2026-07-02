#ifndef MANGO_INSPECTORDRAW_H
#define MANGO_INSPECTORDRAW_H

#include <glm/vec3.hpp>
#include <entt/fwd.hpp>
#include "ECS/Components/ComponentFwd.h"

namespace tomato
{
	struct EditorContext;

	void DrawTransformInspector(EditorContext&, entt::registry&, TransformComponent&);
	
	
	bool DrawVec3Control(const char* label, float* vec);
}


#endif // MANGO_INSPECTORDRAW_H

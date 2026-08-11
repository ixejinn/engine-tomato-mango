#ifndef MANGO_GEOMETRY_H
#define MANGO_GEOMETRY_H

#include "AABB.h"
#include "Ray.h"

namespace tomato
{
	AABB TransformAABB(const AABB& local, const glm::mat4& transform);

	bool IntersectRayAABB(const Ray& ray, const AABB& aabb, float& distance);
}

#endif // !MANGO_GEOMETRY_H

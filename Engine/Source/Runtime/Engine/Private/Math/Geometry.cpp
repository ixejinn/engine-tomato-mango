#include "Math/Geometry.h"

#include <array>
#include <glm/glm.hpp>

namespace tomato
{
	AABB TransformAABB(const AABB& local, const glm::mat4& transform)
	{
		AABB worldAABB;

		bool first = true;

		for (int x = 0; x < 2; x++)
		{
			for (int y = 0; y < 2; y++)
			{
				for (int z = 0; z < 2; z++)
				{
					glm::vec3 corner
					{
						x ? local.max.x : local.min.x,
						y ? local.max.y : local.min.y,
						z ? local.max.z : local.min.z
					};

					glm::vec3 worldCorner = glm::vec3(transform * glm::vec4(corner, 1.f));

					if (first)
					{
						worldAABB.min = worldCorner;
						worldAABB.max = worldCorner;
						first = false;
					}
					else
					{
						worldAABB.min = glm::min(worldAABB.min, worldCorner);
						worldAABB.max = glm::max(worldAABB.max, worldCorner);
					}
				}
			}
		}

		return worldAABB;
	}

	bool IntersectRayAABB(const Ray& ray, const AABB& aabb, float& distance)
	{
		float tMin = 0.f;
		float tMax = FLT_MAX;

		for (int i = 0; i < 3; i++)
		{
			float invD = 1.f / ray.direction[i];

			float t0 = (aabb.min[i] - ray.origin[i]) * invD;
			float t1 = (aabb.max[i] - ray.origin[i]) * invD;

			if (invD < 0.f)
				std::swap(t0, t1);

			tMin = glm::max(tMin, t0);
			tMax = glm::min(tMax, t1);

			if (tMax < tMin)
				return false;
		}

		distance = tMin;

		return true;
	}


}
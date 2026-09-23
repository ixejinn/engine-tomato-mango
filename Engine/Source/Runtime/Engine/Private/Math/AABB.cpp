#include "Math/AABB.h"

namespace tomato
{
    void UpdateAABB(AABB& aabb, const glm::vec3& position, const glm::vec3& halfScale, const glm::mat4& rotation, const float fat)
    {
        glm::vec3 halfExtents
        {
            glm::abs(rotation[0][0]) * halfScale.x + glm::abs(rotation[1][0]) * halfScale.y + glm::abs(rotation[2][0]) * halfScale.z,
            glm::abs(rotation[0][1]) * halfScale.x + glm::abs(rotation[1][1]) * halfScale.y + glm::abs(rotation[2][1]) * halfScale.z,
            glm::abs(rotation[0][2]) * halfScale.x + glm::abs(rotation[1][2]) * halfScale.y + glm::abs(rotation[2][2]) * halfScale.z
        };

        aabb.max = position + halfExtents + fat;
        aabb.min = position - halfExtents - fat;
    }

    bool TestAABBAABB(const AABB& a, const AABB& b)
    {
        if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
        if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
        if (a.max.z < b.min.z || a.min.z > b.max.z) return false;
        return true;
    }
}
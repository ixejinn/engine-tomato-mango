#ifndef MANGO_AABB_H
#define MANGO_AABB_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace tomato
{
    /**
     * @brief Axis-aligned Bounding Box
     */
    struct AABB
    {
        glm::vec3 min;
        glm::vec3 max;
    };

    void UpdateAABB(AABB& aabb, const glm::vec3& position, const glm::vec3& halfScale, const glm::mat4& rotation, float fat = 0);

    bool TestAABBAABB(const AABB& a, const AABB& b);
}
#endif // !MANGO_AABB_H

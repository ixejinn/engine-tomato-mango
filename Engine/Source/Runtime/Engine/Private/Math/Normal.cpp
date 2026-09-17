#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "Math/Normal.h"

#include "Collision/CollisionConfig.h"

namespace tomato
{
    std::optional<glm::vec3> GetOrientedNormal(
            const glm::vec3& refP,
            const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
    {
        const glm::vec3 vec01 = p1 - p0;
        const glm::vec3 vec02 = p2 - p0;
        glm::vec3 normal = glm::cross(vec01, vec02);

        const float lenSqNormal = glm::length2(normal);
        if (lenSqNormal <= RELATIVE_TOLERANCE_SQ * glm::length2(vec01) * glm::length2(vec02))
            return std::nullopt;    // Degeneracy

        normal = glm::normalize(normal);
        return (glm::dot(normal, (refP - p0)) >= 0 ? normal : -normal);
    }

    std::optional<glm::vec3> GetNormal(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
    {
        const glm::vec3 ab = b - a;
        const glm::vec3 ac = c - a;
        glm::vec3 normal = glm::cross(ab, ac);

        const float lenSqNormal = glm::length2(normal);
        if (lenSqNormal <= RELATIVE_TOLERANCE_SQ * glm::length2(ab) * glm::length2(ac))
            return std::nullopt;

        return glm::normalize(normal);
    }
}

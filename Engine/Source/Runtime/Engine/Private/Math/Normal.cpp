#include <glm/glm.hpp>
#include "Math/Normal.h"

namespace tomato {
    glm::vec3 GetOrientedNormal(
            const glm::vec3& refP,
            const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2) {
        const glm::vec3 vec01 = p1 - p0;
        const glm::vec3 vec02 = p2 - p0;
        const glm::vec3 normal = glm::normalize(glm::cross(vec01, vec02));
        return (glm::dot(normal, (refP - p0)) >= 0 ? normal : -normal);
    }
}
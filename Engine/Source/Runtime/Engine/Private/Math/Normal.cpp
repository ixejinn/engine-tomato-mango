#include <glm/glm.hpp>
#include "Math/Normal.h"

namespace tomato {
    glm::vec3 GetOrientedNormal(
            const glm::vec3& refP,
            const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2) {
        const auto vec01 = p1 - p0;
        const auto vec02 = p2 - p0;
        auto normal = glm::cross(vec01, vec02);
        auto x = glm::dot(normal, (refP - p0));
        return (x > 0 ? normal : -normal);
    }
}
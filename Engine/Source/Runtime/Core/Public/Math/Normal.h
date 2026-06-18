#ifndef MANGO_NORMAL_H
#define MANGO_NORMAL_H

#include <glm/vec3.hpp>

namespace tomato {
    glm::vec3 GetOrientedNormal(
            const glm::vec3& refP,
            const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2);
}

#endif //MANGO_NORMAL_H

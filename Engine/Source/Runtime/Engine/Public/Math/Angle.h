#ifndef MANGO_ANGLE_H
#define MANGO_ANGLE_H

#include <glm/fwd.hpp>

namespace tomato
{
    float FindNearAngle(float angle, float ref);
    glm::vec3 GetClosestEulerDegree(const glm::quat& quat, const glm::vec3& eulerDegreeRef);
}

#endif //MANGO_ANGLE_H
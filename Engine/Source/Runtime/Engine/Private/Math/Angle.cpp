#include "Math/Angle.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

namespace tomato
{
    float FindNearAngle(float angle, float ref)
    {
        return angle + 360.f * std::round((ref - angle) / 360.f);
    }

    glm::vec3 GetClosestEulerDegree(const glm::quat& quat, const glm::vec3& eulerDegreeRef)
    {
        glm::vec3 degrees[2];
        degrees[0] = glm::degrees(glm::eulerAngles(quat));    // degA.y [-90, 90]
        degrees[1] = {degrees[0].x + 180.f, 180.f - degrees[0].y, degrees[0].z + 180.f};

        for (auto& degree : degrees)
        {
            degree.x = FindNearAngle(degree.x, eulerDegreeRef.x);
            degree.y = FindNearAngle(degree.y, eulerDegreeRef.y);
            degree.z = FindNearAngle(degree.z, eulerDegreeRef.z);
        }

        auto distSq = [eulerDegreeRef](const glm::vec3& deg)
        {
            glm::vec3 d = deg - eulerDegreeRef;
            return glm::dot(d, d);
        };

        return distSq(degrees[0]) <= distSq(degrees[1]) ? degrees[0] : degrees[1];
    }
}
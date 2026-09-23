#ifndef MANGO_NORMAL_H
#define MANGO_NORMAL_H

#include <optional>
#include <glm/vec3.hpp>

namespace tomato {
    /**
     * @brief Computes the normal vector of the plane defined by three points, oriented toward refP.
     * @return std::optional<glm::vec3> Normal vector pointing toward refP,
     *         or std::nullopt if the points are collinear (degenerate triangle).
     */
    std::optional<glm::vec3> GetOrientedNormal(
            const glm::vec3& refP,
            const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2);

    /**
     * @brief Computes the normal vector of the plane defined by counter-clockwise three points.
     * @return std::optional<glm::vec3> Normal vector pointing toward refP,
     *         or std::nullopt if the points are collinear (degenerate triangle).
     */
    std::optional<glm::vec3> GetNormal(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
}

#endif //MANGO_NORMAL_H

#ifndef MANGO_RAY_H
#define MANGO_RAY_H

#include <glm/glm.hpp>
#include <entt/fwd.hpp>

namespace tomato
{
    struct Ray
    {
        glm::vec3 origin;
        glm::vec3 direction;
    };

    Ray ScreenPointToRay(entt::registry& reg, glm::vec2 mousePos);

}
#endif // !MANGO_RAY_H

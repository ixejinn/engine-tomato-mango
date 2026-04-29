#ifndef MANGO_COLLISIONTESTCOMPONENT_H
#define MANGO_COLLISIONTESTCOMPONENT_H

#include <glm/vec4.hpp>

struct CollisionTestComponent {
    glm::vec4 color;

    constexpr static glm::vec4 COLLISION_COLOR{0.035f, 0.816f, 0.937f, 1.f};
};

#endif //MANGO_COLLISIONTESTCOMPONENT_H

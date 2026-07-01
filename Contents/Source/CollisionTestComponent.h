#ifndef MANGO_COLLISIONTESTCOMPONENT_H
#define MANGO_COLLISIONTESTCOMPONENT_H

#include <glm/vec4.hpp>
#include <optional>

struct CollisionTestComponent {
    std::optional<glm::vec4> color{std::nullopt};

    constexpr static glm::vec4 COLLISION_COLOR{1.f, 0.f, 1.f, 1.f};
};

#endif //MANGO_COLLISIONTESTCOMPONENT_H

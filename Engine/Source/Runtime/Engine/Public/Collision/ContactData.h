#ifndef MANGO_COLLISIONINFO_H
#define MANGO_COLLISIONINFO_H

#include <optional>
#include <glm/vec3.hpp>

namespace tomato
{
    struct ContactData
    {
        ContactData() : trigger(true) {}
        ContactData(glm::vec3 n, float ht, float d) : normal(n), hitTime(ht), distance(d) {}
        ContactData(glm::vec3 n, float d) : normal(n), distance(d) {}

        glm::vec3 normal{0.f};
        std::optional<float> hitTime{std::nullopt};
        float distance{0.f};
        bool trigger{false};
    };
}

#endif //MANGO_COLLISIONINFO_H

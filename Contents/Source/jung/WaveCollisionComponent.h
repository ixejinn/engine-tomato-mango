#ifndef MANGO_WAVECOLLISIONCOMPONENT_H
#define MANGO_WAVECOLLISIONCOMPONENT_H

#include <glm/vec4.hpp>
#include <optional>

struct WaveCollisionComponent {
    std::optional<glm::vec4> color{ std::nullopt };

    constexpr static glm::vec4 COLLISION_COLOR{ 0.f, 0.f, 1.f, 1.f };
};

#endif // !MANGO_WAVECOLLISIONCOMPONENT_H

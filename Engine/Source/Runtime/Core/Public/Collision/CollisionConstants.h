#ifndef MANGO_COLLISIONCONSTANTS_H
#define MANGO_COLLISIONCONSTANTS_H

#include <cstdint>
#include "Serialization/Json.h"

namespace tomato {
    static constexpr float COLLISION_SKIN = 1e-2f;

    enum class CollisionLayer : uint32_t
    {
        Default = 1 << 0,
        COUNT
    };

    enum class ColliderType : uint8_t
    {
        Cube,
        Sphere,
//        Capsule,
        COUNT
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(
        CollisionLayer,
        {
            { CollisionLayer::Default, "Default" }
        }
    )

    NLOHMANN_JSON_SERIALIZE_ENUM(
        ColliderType,
        {
            { ColliderType::Cube, "Cube" },
            { ColliderType::Sphere, "Sphere" }
        }
    )
}

#endif //MANGO_COLLISIONCONSTANTS_H
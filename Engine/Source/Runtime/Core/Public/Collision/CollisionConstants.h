#ifndef MANGO_COLLISIONCONSTANTS_H
#define MANGO_COLLISIONCONSTANTS_H

#include <cstdint>
#include "Serialization/Json.h"

namespace tomato
{
    static constexpr float COLLISION_SKIN = 1e-2f;

#define TMT_COLLISION_LAYER_LIST(X) \
    X(Default, 1 << 0, "Default")           

    enum class CollisionLayer : uint32_t
    {
#define X(Enum, Value, Display) Enum = Value,
        TMT_COLLISION_LAYER_LIST(X)
#undef X
        COUNT
    };
#undef TMT_COLLISION_LAYER_LIST

#define TMT_COLLISION_TYPE_LIST(X)  \
    X(Cube, "Cube")                 \
    X(Sphere, "Sphere")

    enum class ColliderType : uint8_t
    {
#define X(Enum, Display) Enum,
        TMT_COLLISION_TYPE_LIST(X)
#undef X
        COUNT
    };

    struct ColliderTypeMeta
    {
        ColliderType type;
        const char* name;
    };

    static constexpr ColliderTypeMeta ColliderTypeMetas[] =
    {
#define X(Enum, Display) { ColliderType::Enum, Display },
        TMT_COLLISION_TYPE_LIST(X)
#undef X
    };

#undef TMT_COLLISION_TYPE_LIST

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
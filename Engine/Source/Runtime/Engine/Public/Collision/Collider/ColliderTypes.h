#ifndef MANGO_COLLIDERTYPES_H
#define MANGO_COLLIDERTYPES_H

#include <cstdint>
#include "Serialization/Json.h"

namespace tomato
{
    // *---------- Collision type
#define TMT_COLLISION_TYPE_LIST(X)  \
X(Cube, "Cube")                 \
X(Sphere, "Sphere")             \
//X(Capsule, "Capsule")

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
        ColliderType,
        {
            { ColliderType::Cube, "Cube" },
            { ColliderType::Sphere, "Sphere" },
//            { ColliderType::Capsule, "Capsule"}
        }
    )
}

#endif //MANGO_COLLIDERTYPES_H
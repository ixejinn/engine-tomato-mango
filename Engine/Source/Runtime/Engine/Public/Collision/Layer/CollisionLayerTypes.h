#ifndef MANGO_COLLISIONLAYERTYPES_H
#define MANGO_COLLISIONLAYERTYPES_H

#include <cstdint>
#include "Serialization/Json.h"

namespace tomato
{
    // *---------- Collision layer
#define TMT_COLLISION_LAYER_LIST(X) \
X(Default, 1 << 0, "Default")          \
X(Wave1,   1 << 1, "Wave1")            \
X(Wave2,   1 << 2, "Wave2")

    enum class CollisionLayer : uint8_t
    {
#define X(Enum, Value, Display) Enum,
        TMT_COLLISION_LAYER_LIST(X)
#undef X
        COUNT
    };

    struct CollisionLayerMeta
    {
        CollisionLayer layer;
        const char* name;
    };

    static constexpr CollisionLayerMeta CollisionLayerMetas[] =
    {
#define X(Enum, Value, Display) {CollisionLayer::Enum, Display},
        TMT_COLLISION_LAYER_LIST(X)
#undef X
    };

    // *---------- Collision layer bitmask
    enum class CollisionLayerFlag : uint32_t
    {
        None = 0,
#define X(Enum, Value, Display) Enum = Value,
        TMT_COLLISION_LAYER_LIST(X)
#undef X
    };

    inline CollisionLayerFlag GetCollisionLayerFlag(const CollisionLayer layer)
    {
        switch (layer)
        {
#define X(Enum, Value, Display) case CollisionLayer::Enum: return CollisionLayerFlag::Enum;
            TMT_COLLISION_LAYER_LIST(X)
#undef X
        default:
            return CollisionLayerFlag::None;
        }
    }
#undef TMT_COLLISION_LAYER_LIST

    NLOHMANN_JSON_SERIALIZE_ENUM(
        CollisionLayer,
        {
            { CollisionLayer::Default, "Default" },
            { CollisionLayer::Wave1, "Wave1" },
            { CollisionLayer::Wave2, "Wave2" },
        }
    )
}

#endif //MANGO_COLLISIONLAYERTYPES_H
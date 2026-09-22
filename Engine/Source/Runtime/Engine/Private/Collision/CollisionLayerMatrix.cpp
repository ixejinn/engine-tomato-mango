#include <ranges>
#include "Collision/CollisionLayerMatrix.h"
#include "Utils/Bitmask/BitmaskOperators.h"

namespace tomato {
    CollisionLayerMatrix::CollisionLayerMatrix() {
        Initialize();
    }

    void CollisionLayerMatrix::SetCollisionLayer(const CollisionLayer a, const CollisionLayer b, const bool enabled) {
        const CollisionLayerFlag flagA = GetCollisionLayerFlag(a);
        const CollisionLayerFlag flagB = GetCollisionLayerFlag(b);

        if (enabled) {
            matrix_[a] |= flagB;
            matrix_[b] |= flagA;
        }
        else {
            matrix_[a] &= ~flagB;
            matrix_[b] &= ~flagA;
        }
    }

    bool CollisionLayerMatrix::CanCollide(const CollisionLayer a, const CollisionLayer b) {
        return HasFlag(matrix_[a], GetCollisionLayerFlag(b));
    }

    void CollisionLayerMatrix::Initialize() {
        auto layerRange =
                std::views::iota(0, static_cast<int>(CollisionLayer::COUNT)) |
                std::views::transform([](int i) { return static_cast<CollisionLayer>(i); });
        for (CollisionLayer layer : layerRange)
        {
            matrix_[layer] |= CollisionLayerFlag::Default;
            matrix_[CollisionLayer::Default] |= GetCollisionLayerFlag(layer);
        }
    }
}

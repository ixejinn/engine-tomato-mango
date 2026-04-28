#include "Collision/CollisionLayerMatrix.h"
#include "Utils/BitmaskOperators.h"

namespace tomato {
    CollisionLayerMatrix::CollisionLayerMatrix() {
        Initialize();
    }

    void CollisionLayerMatrix::SetCollisionLayer(const CollisionLayer layer1, const CollisionLayer layer2, const bool enabled) {
        if (enabled) {
            matrix_[layer1] |= layer2;
            matrix_[layer2] |= layer1;
        }
        else {
            matrix_[layer1] &= ~layer2;
            matrix_[layer2] &= ~layer1;
        }
    }

    bool CollisionLayerMatrix::CanCollide(const CollisionLayer layer1, const CollisionLayer layer2) {
        return HasFlag(matrix_[layer1], layer2);
    }

    void CollisionLayerMatrix::Initialize() {
        matrix_[CollisionLayer::Default] |= CollisionLayer::Default;
    }
}

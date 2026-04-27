#ifndef MANGO_COLLISIONLAYERMATRIX_H
#define MANGO_COLLISIONLAYERMATRIX_H

#include "Containers/EnumArray.h"
#include "Collision/CollisionConstants.h"

namespace tomato {
	struct CollisionLayerMatrix {
		CollisionLayerMatrix();

		void SetCollisionLayer(CollisionLayer layer1, CollisionLayer layer2, bool enabled);

		bool CanCollide(CollisionLayer layer1, CollisionLayer layer2);

	private:
		void Initialize();

		EnumArray<CollisionLayer, CollisionLayer> matrix_;
	};
}

#endif //MANGO_COLLISIONLAYERMATRIX_H
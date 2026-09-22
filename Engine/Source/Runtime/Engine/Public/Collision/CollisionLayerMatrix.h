#ifndef MANGO_COLLISIONLAYERMATRIX_H
#define MANGO_COLLISIONLAYERMATRIX_H

#include "Containers/EnumArray.h"
#include "Collision/CollisionConstants.h"

namespace tomato
{
	struct CollisionLayerMatrix
	{
		CollisionLayerMatrix();

		void SetCollisionLayer(CollisionLayer a, CollisionLayer b, bool enabled);

		bool CanCollide(CollisionLayer a, CollisionLayer b);

	private:
		void Initialize();

		EnumArray<CollisionLayer, CollisionLayerFlag> matrix_;
	};
}

#endif //MANGO_COLLISIONLAYERMATRIX_H
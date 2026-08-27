#ifndef MANGO_TYPETRAITSTAG_H
#define MANGO_TYPETRAITSTAG_H

namespace tomato
{
	// For verifying which pool it is
	// (prevents errors of returning to the wrong pool upon release)

	template<typename Traits>
	struct PoolOwnerTag {};
}
#endif // !MANGO_TYPETRAITSTAG_H

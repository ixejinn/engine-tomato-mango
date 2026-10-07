#ifndef MANGO_UIDIRTY_H
#define MANGO_UIDIRTY_H

#include <cstdint>
namespace tomato::UI
{
	enum class Dirty : uint8_t
	{
		None = 0,
		Local = 1 << 0,
		Hierarchy = 1 << 1,
		Size = 1 << 2
	};
}

#endif //MANGO_UIDIRTY_H
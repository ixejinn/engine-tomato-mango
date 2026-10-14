#ifndef MANGO_UIDIRTY_H
#define MANGO_UIDIRTY_H

#include <cstdint>
namespace tomato::UI
{
	enum class Dirty : uint8_t
	{
		None = 0,
		Layout = 1 << 0,
		Matrix = 1 << 1,
		All = Layout | Matrix,
	};
}

#endif //MANGO_UIDIRTY_H
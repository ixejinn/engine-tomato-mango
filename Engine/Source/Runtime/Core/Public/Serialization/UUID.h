#ifndef MANGO_UUID_H
#define MANGO_UUID_H
#include <cstdint>

namespace tomato
{
	using UUID = uint64_t;

	UUID GenerateUUID();
}
#endif // !MANGO_UUID_H

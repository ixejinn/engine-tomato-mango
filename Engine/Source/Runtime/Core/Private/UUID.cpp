#include <random>
#include "UUID.h"

namespace tomato
{
	UUID GenerateUUID()
	{
		static std::random_device rd;
		static std::mt19937_64 gen(rd());

		return gen();
	}
}
#ifndef MANGO_NAMETAG_H
#define MANGO_NAMETAG_H

#include <string>
#include "../../Core/Public/UUID.h"

namespace tomato
{
	struct NametagComponent
	{
		UUID id;
		std::string name;
	};
}
#endif // !MANGO_NAMETAG_H

#ifndef MANGO_JSON_IO_H
#define MANGO_JSON_IO_H

#include "Json.h"

namespace tomato::Serialization
{
	void CreateJsonFile(const char*);
	json LoadJsonData(const char*);
}

#endif // !MANGO_JSON_IO_H

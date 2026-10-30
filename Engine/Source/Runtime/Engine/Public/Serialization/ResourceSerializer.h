#ifndef MANGO_RESOURCESERIALIZER_H
#define MANGO_RESOURCESERIALIZER_H

#include "Serialization/Json.h"

namespace tomato::Serialization
{
	class ResourceSerializer
	{
	public:
		void SaveResources(json& root);
		void LoadResources(const json& root);
	};
}

#endif // !MANGO_RESOURCESERIALIZER_H
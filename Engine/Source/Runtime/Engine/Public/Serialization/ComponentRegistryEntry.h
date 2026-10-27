#ifndef MANGO_COMPONENTREGISTRYENTRY_H
#define MANGO_COMPONENTREGISTRYENTRY_H

#include <string>

#include "ComponentRegistry.h"

namespace tomato::Serialization
{
	template<typename T>
	struct ComponentRegistryEntry
	{
		ComponentRegistryEntry(
			const std::string& name,
			ComponentCategory category,
			ComponentFlags flags)
		{
			ComponentRegistry::GetInstance().RegisterComponent<T>(name, category, flags);
		}

		ComponentRegistryEntry(
			const std::string& name,
			ComponentCategory category,
			ComponentFlags flags,
			SaveFunc save,
			LoadFunc load)
		{
			ComponentRegistry::GetInstance().RegisterComponent<T>(
				name, category, flags, std::move(save), std::move(load));
		}
	};
}

//#define REGISTER_COMPONENT()
#endif // !MANGO_COMPONENTREGISTRYENTRY_H

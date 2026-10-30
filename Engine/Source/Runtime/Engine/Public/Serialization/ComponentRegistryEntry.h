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
			ComponentFlags flags,
			bool serializable)
		{
			ComponentRegistry::GetInstance().RegisterComponent<T>(name, category, flags, serializable);
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

#define TMT_CONCAT_(a,b) a##b
#define TMT_CONCAT(a,b) TMT_CONCAT_(a,b)

#define REGISTER_COMPONENT(TYPE, NAME, CATEGORY, FLAGS, SERIALIZABLE)\
namespace { tomato::Serialization::ComponentRegistryEntry<TYPE> \
	TMT_CONCAT(g_entry_, __COUNTER__){ NAME, CATEGORY, FLAGS, SERIALIZABLE }; }

#define REGISTER_SERIALIZABLE_COMPONENT(TYPE, NAME, CATEGORY, FLAGS, SAVE, LOAD)\
namespace { tomato::Serialization::ComponentRegistryEntry<TYPE> \
	TMT_CONCAT(g_entry_, __COUNTER__){ NAME, CATEGORY, FLAGS, true, SAVE, LOAD }; }

#endif // !MANGO_COMPONENTREGISTRYENTRY_H

#ifndef MANGO_COMPONENTREGISTRY_H
#define MANGO_COMPONENTREGISTRY_H

#include <vector>
#include <string>
#include <entt/entt.hpp>

#include "Serialization/ComponentInfo.h"
#include "Serialization/ComponentSerializer.h"

namespace tomato::Serialization
{
	struct ComponentRegistry
	{
		ComponentRegistry() = default;

	public:

		ComponentRegistry(const ComponentRegistry&) = delete;
		ComponentRegistry& operator=(const ComponentRegistry&) = delete;

		static ComponentRegistry& GetInstance()
		{
			static ComponentRegistry instance;
			return instance;
		}

		void Init();

		template<typename T>
		void RegisterComponent(const std::string& name);

		const auto& GetInfo() const
		{
			return componentInfo_;
		}

	private:
		bool initialized = false;
		std::vector<ComponentInfo> componentInfo_;
	};

	template<typename T>
	void tomato::Serialization::ComponentRegistry::RegisterComponent(const std::string& name)
	{
		ComponentInfo info;

		info.name = name;

		info.Has =
			[](entt::registry& reg, entt::entity e)
			{
				return reg.any_of<T>(e);
			};

		info.Save =
			[](json& j, entt::registry& reg, entt::entity e)
			{
				Serialization::Save(j, reg.get<T>(e));
			};

		info.Load =
			[](const json& j, entt::registry& reg, entt::entity e)
			{
				T component;

				Serialization::Load(j, component);
				reg.emplace<T>(e, std::move(component));
			};

		componentInfo_.push_back(info);
	}
}

#endif // MANGO_COMPONENTREGISTRY_H

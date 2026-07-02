#ifndef MANGO_COMPONENTREGISTRY_H
#define MANGO_COMPONENTREGISTRY_H

#include <vector>
#include <string>
#include <unordered_map>
#include <entt/entt.hpp>

#include "Serialization/ComponentInfo.h"
#include "Serialization/ComponentSerializer.h"

#include "InspectorDraw.h"

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
		void InitInspector();

		template<typename T>
		void RegisterComponent(const std::string& name);

		template<typename T>
		void RegisterInspector(const std::string& name, void (*draw)(EditorContext&, entt::registry&, T&));

		const auto& GetComponentInfo() const
		{
			return componentInfo_;
		}

		const ComponentInfo* FindComponentInfo(const std::string& name) const;

	private:
		bool initialized = false;
		std::vector<ComponentInfo> componentInfo_;
		std::unordered_map<std::string, size_t> nameToIndex_;
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

		if constexpr (std::is_empty_v<T>)
		{
			info.Save =
				[](json&, entt::registry&, entt::entity)
				{
				};

			info.Load =
				[](const json&, entt::registry& reg, entt::entity e)
				{
					reg.emplace<T>(e);
				};
		}
		else
		{
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
		}

		nameToIndex_[name] = componentInfo_.size();
		componentInfo_.push_back(std::move(info));
	}

	template<typename T>
	void tomato::Serialization::ComponentRegistry::RegisterInspector
		(const std::string& name, void (*draw)(EditorContext&, entt::registry&, T&))
	{
		auto& component = componentInfo_[nameToIndex_[name]];
		component.Draw =
			[draw](EditorContext& eCtx, entt::registry& reg, entt::entity e)
			{
				draw(eCtx, reg, reg.get<T>(e));
			};
	}
}

#endif // MANGO_COMPONENTREGISTRY_H

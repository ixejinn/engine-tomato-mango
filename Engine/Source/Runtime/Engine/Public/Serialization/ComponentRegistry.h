#ifndef MANGO_COMPONENTREGISTRY_H
#define MANGO_COMPONENTREGISTRY_H

#include <vector>
#include <string>
#include <typeindex>
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
		void RegisterComponent(const std::string& name, ComponentCategory category, ComponentFlags flags = (ComponentFlags)0, bool serializable = true);

		template<typename T>
		void RegisterComponent(const std::string& name, ComponentCategory category, ComponentFlags flags, bool serializable, SaveFunc save, LoadFunc load);

		template<typename T>
		void RegisterInspector(const std::string& name, bool (*draw)(EditorContext&, entt::registry&, T&));

		const auto& GetComponentInfo() const
		{
			return componentInfo_;
		}

		const ComponentInfo* FindComponentInfo(const std::string& name) const;

	private:
		bool HasComponentName(const std::string& name) const;

		template<typename T>
		bool HasComponentType() const;

	private:
		bool initialized = false;
		std::vector<ComponentInfo> componentInfo_;
		std::unordered_map<std::string, size_t> nameToIndex_;
		std::unordered_map<std::type_index, size_t> typeToIndex_;
	};

	template<typename T>
	void tomato::Serialization::ComponentRegistry::RegisterComponent(const std::string& name, ComponentCategory category, ComponentFlags flags, bool serializable)
	{
		const std::type_index type = typeid(T);
		if (HasComponentType<T>())
		{
			TMT_ERR << "Component type: " << type.name() << " already registered.";
			return;
		}

		if (HasComponentName(name))
		{
			TMT_ERR << "Component name: " << name << " already registered.";
			return;
		}

		ComponentInfo info;

		info.name = name;
		info.category = category;
		info.flags = flags;
		info.serializable = serializable;

		info.Has =
			[](entt::registry& reg, entt::entity e)
			{
				return reg.any_of<T>(e);
			};

		info.editor.Add =
			[](entt::registry& reg, entt::entity e)
			{
				static_cast<void>(reg.get_or_emplace<T>(e));	
			};

		info.editor.Remove =
			[](entt::registry& reg, entt::entity e)
			{
				if (reg.any_of<T>(e))
					reg.remove<T>(e);
			};

		if (serializable)
		{
			if constexpr (std::is_empty_v<T>)
			{
				info.serialization.Save =
					[](json&, entt::registry&, entt::entity)
					{
					};

				info.serialization.Load =
					[](const json&, entt::registry& reg, entt::entity e)
					{
						reg.emplace_or_replace<T>(e);
					};
			}
			else
			{
				info.serialization.Save =
					[](json& j, entt::registry& reg, entt::entity e)
					{
						Serialization::Save(j, reg, reg.get<T>(e));
					};

				info.serialization.Load =
					[](const json& j, entt::registry& reg, entt::entity e)
					{
						T component;

						Serialization::Load(j, reg, component);
						reg.emplace<T>(e, std::move(component));
					};
			}
		}

		nameToIndex_[name] = componentInfo_.size();
		typeToIndex_[type] = componentInfo_.size();
		componentInfo_.push_back(std::move(info));
	}

	template<typename T>
	void tomato::Serialization::ComponentRegistry::RegisterComponent(const std::string& name, ComponentCategory category, ComponentFlags flags, bool serializable, SaveFunc save, LoadFunc load)
	{
		const std::type_index type = typeid(T);
		if (HasComponentType<T>())
		{
			TMT_ERR << "Component type: " << type.name() << " already registered.";
			return;
		}

		if (HasComponentName(name))
		{
			TMT_ERR << "Component name: " << name << " already registered.";
			return;
		}

		ComponentInfo info;

		info.name = name;
		info.category = category;
		info.flags = flags;
		info.serializable = serializable;

		info.Has =
			[](entt::registry& reg, entt::entity e)
			{
				return reg.any_of<T>(e);
			};

		info.editor.Add =
			[](entt::registry& reg, entt::entity e)
			{
				static_cast<void>(reg.get_or_emplace<T>(e));
			};

		info.editor.Remove =
			[](entt::registry& reg, entt::entity e)
			{
				if (reg.any_of<T>(e))
					reg.remove<T>(e);
			};

		if (serializable)
		{
			info.serialization.Save = save;
			info.serialization.Load = load;
		}

		nameToIndex_[name] = componentInfo_.size();
		typeToIndex_[type] = componentInfo_.size();
		componentInfo_.push_back(std::move(info));
	}

	template<typename T>
	void tomato::Serialization::ComponentRegistry::RegisterInspector
		(const std::string& name, bool (*draw)(EditorContext&, entt::registry&, T&))
	{
		auto& component = componentInfo_[nameToIndex_[name]];
		component.editor.Draw =
			[draw](EditorContext& eCtx, entt::registry& reg, entt::entity e)
			{
				return draw(eCtx, reg, reg.get<T>(e));
			};
	}

	template<typename T>
	bool tomato::Serialization::ComponentRegistry::HasComponentType() const
	{
		return typeToIndex_.contains(std::type_index(typeid(T)));
	}
}

#endif // MANGO_COMPONENTREGISTRY_H

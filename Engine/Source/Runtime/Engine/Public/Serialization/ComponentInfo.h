#ifndef MANGO_COMPONENTINFO_H
#define MANGO_COMPONENTINFO_H

#include <string>
#include <functional>
#include <entt/fwd.hpp>
#include "Serialization/Json.h"
#include "EnumFlags.h"

namespace tomato
{
	struct EditorContext;
}

namespace tomato::Serialization
{

#define TMT_COMPONENT_CATEGORY_LIST(X)	\
	X(Common, "Common")					\
	X(UI, "UI")							\
	X(Tag, "Tag")

	enum class ComponentCategory
	{
#define X(Enum, Display) Enum,
		TMT_COMPONENT_CATEGORY_LIST(X)
#undef X
		COUNT
	};

	struct ComponentCategoryMeta
	{
		ComponentCategory category;
		const char* name;
	};

	inline constexpr ComponentCategoryMeta componentCategoryMetas[] =
	{
#define X(Enum, Display) { ComponentCategory::Enum, Display },
		TMT_COMPONENT_CATEGORY_LIST(X)
#undef X
	};
#undef TMT_COMPONENT_CATEGORY_LIST


#define TMT_COMPONENT_FLAGS_LIST(X)		\
	X(None, "None", 0)					\
	X(Hidden, "Hidden", 1 << 0)			\
	X(ReadOnly, "ReadOnly", 1 << 1)		\
	X(Essential, "Essential", 1 << 2)	

	enum class ComponentFlags : uint32_t
	{
#define X(Enum, Display, Value) Enum = Value,
		TMT_COMPONENT_FLAGS_LIST(X)
#undef X
		COUNT
	};
	TMT_ENABLE_ENUM_FLAGS(ComponentFlags)

	struct ComponentFlagsMeta
	{
		ComponentFlags category;
		const char* name;
	};

	inline constexpr ComponentFlagsMeta componentFlagsMetas[] =
	{
#define X(Enum, Display, Value) { ComponentFlags::Enum, Display },
		TMT_COMPONENT_FLAGS_LIST(X)
#undef X
	};
#undef TMT_COMPONENT_FLAGS_LIST

	struct ComponentSerializationInfo
	{
		void (*Save)(json&, entt::registry&, entt::entity);
		void (*Load)(const json&, entt::registry&, entt::entity);
	};

	using DrawInspector = std::function<bool(EditorContext&, entt::registry&, entt::entity)>;
	struct ComponentEditorInfo
	{
		DrawInspector Draw;
		void (*Add)(entt::registry&, entt::entity);
		void (*Remove)(entt::registry&, entt::entity);
	};

	struct ComponentInfo
	{
		std::string name;

		ComponentCategory category;
		ComponentFlags flags;

		bool (*Has)(entt::registry&, entt::entity);
		
		ComponentSerializationInfo serialization;
		ComponentEditorInfo editor;
	};
}
#endif // !MANGO_COMPONENTINFO_H

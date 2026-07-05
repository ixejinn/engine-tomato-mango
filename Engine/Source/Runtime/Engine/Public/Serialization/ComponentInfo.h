#ifndef MANGO_COMPONENTINFO_H
#define MANGO_COMPONENTINFO_H

#include <string>
#include <functional>
#include <entt/fwd.hpp>
#include "Serialization/Json.h"

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

	struct ComponentSerializationInfo
	{
		void (*Save)(json&, entt::registry&, entt::entity);
		void (*Load)(const json&, entt::registry&, entt::entity);
	};

	using DrawInspector = std::function<void(EditorContext&, entt::registry&, entt::entity)>;
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

		bool (*Has)(entt::registry&, entt::entity);
		
		ComponentSerializationInfo serialization;
		ComponentEditorInfo editor;
	};
}
#endif // !MANGO_COMPONENTINFO_H

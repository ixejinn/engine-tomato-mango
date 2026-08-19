#ifndef MANGO_UIPREFAB_H
#define MANGO_UIPREFAB_H

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <string>
#include "Resource/Render/Texture.h"
#include "Resource/Render/Font.h"
#include "ECS/Components/UI.h"

namespace tomato::UIPrefab
{
	entt::entity CreateCanvas(entt::registry& reg, RenderMode mode = RenderMode::ScreenOverlay);

	entt::entity CreateButton(
		entt::registry& reg,
		entt::entity canvas = entt::null,
		glm::vec2		pos = {0.f, 0.f}
	);

	entt::entity CreateText(
		entt::registry& reg,
		entt::entity canvas = entt::null,
		glm::vec2		pos = { 0.f, 0.f },
		std::string		inText = "Text",
		glm::vec4		color = { 1.f, 1.f, 1.f, 1.f },
		float			size = 12.f,
		//const char*		fontName = Font::defaultFontPath
		const std::filesystem::path&		fontName = Font::defaultFontPath
	);

	entt::entity CreateImage(
		entt::registry& reg,
		entt::entity canvas = entt::null,
		//const char*		textureName = Texture::PrimitiveName,
		const std::filesystem::path& textureName = Texture::PrimitiveName,
		glm::vec2		pos = { 0.f, 0.f },
		glm::vec2		size = { 0.f, 0.f }
	);

	entt::entity GetCanvas(entt::registry& reg);

}

#endif // !MANGO_UIPREFAB_H

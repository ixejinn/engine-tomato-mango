#ifndef MANGO_UIPREFAB_H
#define MANGO_UIPREFAB_H

#include <entt/fwd.hpp>
#include <glm/glm.hpp>
#include <string>
#include "Resource/Render/Texture.h"

namespace tomato
{
	entt::entity CreateCanvas(entt::registry& reg);

	entt::entity CreateButton(entt::registry& reg);

	entt::entity CreateText(
		entt::registry& reg,
		glm::vec2		pos = { 0.f, 0.f },
		std::string		inText = "Text",
		glm::vec4		color = { 1.f, 1.f, 1.f, 1.f },
		float			size = 12.f
	);

	entt::entity CreateImage(
		entt::registry& reg,
		const char*		textureName = Texture::PrimitiveName,
		glm::vec2		pos = { 0.f, 0.f },
		glm::vec2		size = { 0.f, 0.f }
	);

	entt::entity GetCanvas(entt::registry& reg);

}

#endif // !MANGO_UIPREFAB_H

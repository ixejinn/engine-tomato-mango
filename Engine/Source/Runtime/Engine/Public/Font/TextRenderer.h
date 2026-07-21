#ifndef MANGO_TEXT_RENDERER_H
#define MANGO_TEXT_RENDERER_H

#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "Resource/Render/Font.h"

namespace tomato
{
	struct TextVertex
	{
		glm::vec2 position;
		glm::vec2 uv;
		glm::vec4 color;
	};

	class Shader;
	class TextRenderer
	{
	public:
		~TextRenderer();
		
		void Init(Shader* shader);

		// Add text data to the buffer
		void DrawString(const std::u32string& text, float x, float y, float size, const glm::vec4& color, Font* font, const glm::mat4 model);
		
		// Actually send data to GPU and Draw
		void Flush();

		int GetVertexCount() { return vertices_.size(); }
	private:
		// Helper to add a qua (2 triangles, 6 vertices)
		void AddQuad(float x, float y, float size, const Glyph& glyph, const glm::vec4& color, const glm::mat4 model);

	private:
		Shader* shader_{ nullptr };

		GLuint vao_{ 0 }, vbo_{ 0 };
		int currentAtlasIndex_{ -1 };

		std::vector<TextVertex> vertices_;
	};
}

#endif // !MANGO_TEXT_RENDERER_H
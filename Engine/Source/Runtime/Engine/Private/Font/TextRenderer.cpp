#include <glm/ext.hpp>
#include <glad/glad.h>

#include "Font/TextRenderer.h"
#include "Font/AtlasManager.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Render/Shader.h"
#include "Utils/Utf.h"

namespace tomato
{
	TextRenderer::~TextRenderer()
	{
		if (vao_ != 0) glDeleteVertexArrays(1, &vao_);
		if (vbo_ != 0) glDeleteBuffers(1, &vbo_);
	}

	void TextRenderer::Init(Shader* shader)
	{
		shader_ = shader;

		shader_->Use();

		glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(1600), 0.0f, static_cast<float>(900), -1.0f, 1.0f);
		shader->SetUniformMat4("projection", projection);

		glUseProgram(0);

		// 1. Create and bind VAO
		glGenVertexArrays(1, &vao_);
		glGenBuffers(1, &vbo_);
		glBindVertexArray(vao_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);

		// 2. Pre-allocate VBO memory (e.g., space for 1000 characters)
		// 1 char = 6 vertices (2 triangles)
		size_t reservedSize = sizeof(TextVertex) * 6 * 1000;
		glBufferData(GL_ARRAY_BUFFER, reservedSize, nullptr, GL_DYNAMIC_DRAW);

		// 3. Set Vertex Attributes
		// Position (vec2)
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, position));
	
		// UV (vec2)
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, uv));

		// Color (vec4)
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, color));

		glBindVertexArray(0);
		
		TMT_INFO << "TextRenderer initialized with reserved capacity: 1000 chars.";
	}

	void TextRenderer::DrawString(const std::u32string& text, float x, float y, float size, const glm::vec4& color, Font* font, const glm::mat4 model)
	{
		// @NOTE :
		// Pixel perfect handling is not implemented yet.
		// May cause minor visual issues (e.g. blurriness or jitter).
		
		std::u32string::const_iterator c;

		for (c = text.begin(); c != text.end(); c++)
		{
			const Glyph& glyph = font->GetGlyph(*c);

			// If the atlas changes, draw the current batch and clear it (Flush).
			if (currentAtlasIndex_ != -1 && currentAtlasIndex_ != glyph.atlasIndex)
				Flush();

			currentAtlasIndex_ = glyph.atlasIndex;

			float xpos = x + glyph.bearing.x * size;
			float ypos = y - (glyph.size.y - glyph.bearing.y) * size;
			float w = glyph.size.x * size;
			float h = glyph.size.y * size;

			float u1 = glyph.uvMin.x;
			float v1 = glyph.uvMin.y;
			float u2 = glyph.uvMax.x;
			float v2 = glyph.uvMax.y;

			// Generate 6 vertices for two triangles (TL, BL, BR, TL, BR, TR) to form a quad.
			AddQuad(x, y, size, glyph, color, model);

			// Advance the cursor position for the next character.
			x += glyph.advance * size;
		}
	}

	void TextRenderer::Flush()
	{
		if (vertices_.empty()) return;

		AtlasManager::GetInstance().BindAtlas(currentAtlasIndex_);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_.size() * sizeof(TextVertex), vertices_.data());

		glBindVertexArray(vao_);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices_.size()));

		vertices_.clear();
	}

	void TextRenderer::AddQuad(float x, float y, float size, const Glyph& glyph, const glm::vec4& color, const glm::mat4 model)
	{
		float xpos = x + glyph.bearing.x * size;
		float ypos = y - (glyph.size.y - glyph.bearing.y) * size;
		float w = glyph.size.x * size;
		float h = glyph.size.y * size;

		float u1 = glyph.uvMin.x;
		float v1 = glyph.uvMin.y;
		float u2 = glyph.uvMax.x;
		float v2 = glyph.uvMax.y;

		vertices_.emplace_back(TextVertex{ model * glm::vec4(xpos, ypos + h, 0.0, 1.0),		{u1, v1}, color });
		vertices_.emplace_back(TextVertex{ model * glm::vec4(xpos, ypos, 0.0, 1.0),			{u1, v2}, color });
		vertices_.emplace_back(TextVertex{ model * glm::vec4(xpos + w, ypos, 0.0, 1.0),		{u2, v2}, color });

		vertices_.emplace_back(TextVertex{ model * glm::vec4(xpos, ypos + h, 0.0, 1.0),		{u1, v1}, color });
		vertices_.emplace_back(TextVertex{ model * glm::vec4(xpos + w, ypos, 0.0, 1.0),		{u2, v2}, color });
		vertices_.emplace_back(TextVertex{ model * glm::vec4(xpos + w, ypos + h, 0.0, 1.0),	{u2, v1}, color });
	}
}
#ifndef MANGO_GLSTATECACHE_H
#define MANGO_GLSTATECACHE_H

#include <cstdint>
#include <array>
#include <glad/glad.h>

namespace tomato
{
	struct DepthState
	{
		bool testEnabled = true;
		bool writeEnabled = true;
		GLenum func = GL_LESS;

		bool operator==(const DepthState&) const = default; // !=는 자동으로 생성
	};

	struct StencilState
	{
		bool enabled = false;
		GLenum func = GL_ALWAYS;
		uint8_t ref = 0;
		uint8_t readMask = 0xFF;
		uint8_t writeMask = 0xFF;
		GLenum sfail = GL_KEEP;
		GLenum dpfail = GL_KEEP;
		GLenum dppass = GL_KEEP;

		bool operator==(const StencilState&) const = default;
	};

	struct RasterState
	{
		bool cullEnabled = true;
		GLenum cullFace = GL_BACK;
		GLenum frontFace = GL_CCW;

		bool operator==(const RasterState&) const = default;
	};

	struct BlendState
	{
		bool enabled = false;
		GLenum srcColor = GL_SRC_ALPHA;
		GLenum dstColor = GL_ONE_MINUS_SRC_ALPHA;
		GLenum srcAlpha = GL_SRC_ALPHA;
		GLenum dstAlpha = GL_ONE_MINUS_SRC_ALPHA;
		GLenum equation = GL_FUNC_ADD;

		bool operator==(const BlendState&) const = default;
	};

	struct PipelineState
	{
		DepthState depth;
		StencilState stencil;
		RasterState raster;
		BlendState blend;

		bool operator==(const PipelineState&) const = default;
	};

	namespace PipelinePresets
	{
		inline constexpr PipelineState Default {};

        // RenderSystem의 기본 3D 렌더 상태
        inline constexpr PipelineState Opaque
        {
            .stencil = {.enabled = true, .dppass = GL_REPLACE },
            .blend = {.enabled = true },
        };

		inline constexpr PipelineState Transparent
		{
			.depth = {.testEnabled = true, .writeEnabled = false, .func = GL_LESS },
			.blend = {.enabled = true },
		};

		inline constexpr PipelineState Skybox
		{
			.depth = {.testEnabled = true, .writeEnabled = false, .func = GL_LEQUAL },
			.raster = {.cullEnabled = true, .cullFace = GL_FRONT },
			.blend = {.enabled = true },
		};

		inline constexpr PipelineState ScreenUI
		{

		};
	}

	class GLStateCache
	{
	public:
		GLStateCache();

		void Apply(const PipelineState& state);

		bool UseShader(GLuint program);
		void BindVertexArray(GLuint vao);
		void BindTexture(GLuint tex, GLuint unit = 0);

		void SetViewport(int x, int y, int w, int h);

		void Clear(GLbitfield flags);
		
		void Invalidate();

	private:
		void ApplyDepth(const DepthState& depth);
		void ApplyStencil(const StencilState& stencil);
		void ApplyRaster(const RasterState& raster);
		void ApplyBlend(const BlendState& blend);

		void ForceApply(const PipelineState& state); // 기본 값으로 강제 적용, Invalidate에서 호출
	private:
		PipelineState current_;

		// 현재 GL에 바인딩된 핸들
		GLuint program_{ 0 };
		GLuint vao_{ 0 };
		std::array<GLuint, 16> textures_{}; // 현재는 unit 0번 밖에 안 쓰지만 확장 고려해서 array 사용
	};
}
#endif // !MANGO_GLSTATECACHE_H

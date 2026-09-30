#include "Render/GLStateCache.h"

namespace tomato
{
	void GLStateCache::Apply(const PipelineState& state)
	{
		if (state == current_) return;

		ApplyDepth(state.depth);
	}

	void GLStateCache::UseShader(GLuint program)
	{
		if (program_ == program) return;

		glUseProgram(program);
		program_ = program;
	}

	void GLStateCache::BindVertexArray(GLuint vao)
	{
		if (vao_ == vao) return;

		glBindVertexArray(vao);
		vao_ = vao;
	}

	void GLStateCache::BindTexture(GLuint tex, GLuint unit)
	{
		if (unit >= textures_.size()) return;
		if (textures_[unit] == tex) return;
		glBindTextureUnit(unit, tex);
		textures_[unit] = tex;
	}

	void GLStateCache::SetViewport(int x, int y, int w, int h)
	{
		glViewport(x, y, w, h);
	}

	void GLStateCache::Clear(GLbitfield flags)
	{
		glClear(flags);
	}

	void GLStateCache::ApplyDepth(const DepthState& d)
	{
		DepthState& c = current_.depth;
		if (d == c) return;

		if (d.testEnabled != c.testEnabled)
			d.testEnabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);

		//테스트가 켜져 있을 때만 설정
		if (d.testEnabled && (d.func != c.func || !c.testEnabled))
			glDepthFunc(d.func);

		if (d.writeEnabled != c.writeEnabled)
			glDepthMask(d.writeEnabled ? GL_TRUE : GL_FALSE);

		c = d;
	}
}
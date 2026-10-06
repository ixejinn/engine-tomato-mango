#include "Render/GLStateCache.h"

namespace tomato
{
	GLStateCache::GLStateCache()
	{
		Invalidate();
	}

	void GLStateCache::Apply(const PipelineState& state)
	{
		if (state == current_) return;

		ApplyDepth(state.depth);
		ApplyStencil(state.stencil);
		ApplyRaster(state.raster);
		ApplyBlend(state.blend);
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
		if ((flags & GL_DEPTH_BUFFER_BIT) && !current_.depth.writeEnabled)
		{
			glDepthMask(GL_TRUE);
			current_.depth.writeEnabled = true;
		}
		if ((flags & GL_STENCIL_BUFFER_BIT) && current_.stencil.writeMask != 0xFF)
		{
			glStencilMask(0xFF);
			current_.stencil.writeMask = 0xFF;
		}

		glClear(flags);
	}

	void GLStateCache::Invalidate()
	{
		ForceApply(PipelineState{PipelinePresets::Default3D});

		glUseProgram(0);
		glBindVertexArray(0);
		glBindTextures(0, static_cast<GLsizei>(textures_.size()), nullptr);
		
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		program_ = 0;
		vao_ = 0;
		textures_.fill(0);
	}

	void GLStateCache::ApplyDepth(const DepthState& d)
	{
		DepthState& c = current_.depth;
		if (d == c) return;

		if (d.testEnabled != c.testEnabled)
			d.testEnabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);

		// 테스트가 켜져 있을 때만 설정
		if (d.testEnabled && (d.func != c.func || !c.testEnabled))
			glDepthFunc(d.func);

		if (d.writeEnabled != c.writeEnabled)
			glDepthMask(d.writeEnabled ? GL_TRUE : GL_FALSE);

		c = d;
	}
	void GLStateCache::ApplyStencil(const StencilState& s)
	{
		StencilState& c = current_.stencil;
		if (s == c) return;

		s.enabled ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
		glStencilFunc(s.func, s.ref, s.readMask);
		glStencilOp(s.sfail, s.dpfail, s.dppass);
		glStencilMask(s.writeMask);

		c = s;
	}

	void GLStateCache::ApplyRaster(const RasterState& r)
	{
		RasterState& c = current_.raster;
		if (r == c) return;

		r.cullEnabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
		glCullFace(r.cullFace);
		glFrontFace(r.frontFace);

		c = r;
	}

	void GLStateCache::ApplyBlend(const BlendState& b)
	{
		BlendState& c = current_.blend;
		if (b == c) return;

		b.enabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
		glBlendFuncSeparate(b.srcColor, b.dstColor, b.srcAlpha, b.dstAlpha);
		glBlendEquation(b.equation);

		c = b;
	}

	void GLStateCache::ForceApply(const PipelineState& state)
	{
		state.depth.testEnabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
		glDepthFunc(state.depth.func);
		glDepthMask(state.depth.writeEnabled ? GL_TRUE : GL_FALSE);

		state.stencil.enabled ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
		glStencilFunc(state.stencil.func, state.stencil.ref, state.stencil.readMask);
		glStencilOp(state.stencil.sfail, state.stencil.dpfail, state.stencil.dppass);
		glStencilMask(state.stencil.writeMask);

		state.raster.cullEnabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
		glCullFace(state.raster.cullFace);
		glFrontFace(state.raster.frontFace);

		state.blend.enabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
		glBlendFuncSeparate(state.blend.srcColor, state.blend.dstColor, state.blend.srcAlpha, state.blend.dstAlpha);
		glBlendEquation(state.blend.equation);

		current_ = state;
	}

}
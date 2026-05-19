#ifndef MANGO_SCREENUI_RENDER_SYSTEM_H
#define MANGO_SCREENUI_RENDER_SYSTEM_H

#include "ECS/Systems/System.h"
#include "Resource/ResourceFwd.h"
#include "Font/TextRenderer.h"

namespace tomato
{
	class ScreenUIRenderSystem : public System
	{
	public:
		ScreenUIRenderSystem();

		void Update(SimContext& simCtx) override;

	private:
		TextRenderer textRenderer_;

		AssetID curMesh_;
		AssetID curShader_;
		AssetID curTexture_;
	};
}

#endif // !MANGO_SCREENUI_RENDER_SYSTEM_H

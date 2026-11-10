#ifndef MANGO_NETWORKPANEL_H
#define MANGO_NETWORKPANEL_H

#include "EditorPanel.h"

namespace tomato
{
	class NetworkPanel : public EditorPanel
	{
	public:
		NetworkPanel(float width, float height, float x, float y);

		void Draw(EditorContext&) override;
		const char* GetName() const { return "NetworkPanel"; }

	private:
		void DrawNetworkModeButton();
		void DrawClientModeButton();
		void DrawServerModeButton();
		void DrawHostModeButton();

	private:
		bool client{ false };
		bool server{ false };
	};
}
#endif // !MANGO_NETWORKPANEL_H

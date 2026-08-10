#ifndef MANGO_EDITORPANEL_H
#define MANGO_EDITORPANEL_H

#include <filesystem>
#include <entt/fwd.hpp>
#include <glm/glm.hpp>
#include "State/StateFwd.h"

namespace tomato
{
	struct EditorContext
	{
		State* currentState;
		entt::entity selectedEntity;

		// Scene
		std::filesystem::path currentScenePath;
		std::filesystem::path currentSceneRuntimePath;
		bool sceneDirty = false;
	};

	class EditorPanel
	{
	public:
		EditorPanel(float width, float height, float x, float y)
			: width_(width), height_(height),
			posX_(x), posY_(y) {}

		virtual const char* GetName() const = 0;
		virtual void Draw(EditorContext&) = 0;

		void SetSize(glm::vec2 size) { width_ = size.x; height_ = size.y; }
		void SetPos(glm::vec2 pos) { posX_ = pos.x; posY_ = pos.y; }
		void SetPivot(glm::vec2 pivot) { pivot_ = pivot; }

		const glm::vec2 GetSize() { return glm::vec2{ width_, height_ }; }
		const glm::vec2 GetPos() { return glm::vec2{ posX_, posY_ }; }
		const glm::vec2 GetPivot() { return pivot_; }

		bool open{ false };

	protected:
		float width_{ 0 };
		float height_{ 0 };

		float posX_{ 0 };
		float posY_{ 0 };

		glm::vec2 pivot_{ 1.f, 1.f };
	};
	
}
#endif // !MANGO_EDITORPANEL_H

#include "ECS/Components/UI.h"
#include "ECS/Components/UIDirty.h"
#include "Utils/Bitmask/BitmaskOperators.h"
namespace tomato
{
	void RectTransformComponent::SetPosition(const glm::vec2& pos)
	{
		if (anchoredPosition == pos)
			return;

		anchoredPosition = pos;

		dirty |= UI::Dirty::Layout;
	}

	void RectTransformComponent::SetOffsetMin(const glm::vec2& min)
	{
		if (offsetMin == min)
			return;

		offsetMin = min;

		dirty |= UI::Dirty::Layout;
	}

	void RectTransformComponent::SetOffsetMax(const glm::vec2& max)
	{
		if (offsetMax == max)
			return;

		offsetMax = max;

		dirty |= UI::Dirty::Layout;
	}

	void RectTransformComponent::SetSize(const glm::vec2& size)
	{
		if (sizeDelta == size)
			return;

		sizeDelta = size;

		dirty |= UI::Dirty::Layout;
	}

	void RectTransformComponent::SetAnchor(const glm::vec2& min, const glm::vec2& max)
	{
		if (anchorMin == min && anchorMax == max)
			return;
		
		anchorMin = min;
		anchorMax = max;

		dirty |= UI::Dirty::Layout;
	}

	void RectTransformComponent::SetPivot(const glm::vec2& pvt)
	{
		if (pivot == pvt)
			return;

		pivot = pvt;

		dirty |= UI::Dirty::Layout;
	}
}
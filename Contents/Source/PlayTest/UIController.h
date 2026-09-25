#ifndef MANGO_UICONTROLLER_H
#define MANGO_UICONTROLLER_H

#include "ECS/Components/UIEvents.h"

class UIController
{
public:
	void onClick(const tomato::MouseClickEvent& mouseEvt);
	void onMatchRequest(const tomato::MouseClickEvent& mouseEvt);
};

#endif // !MANGO_UICONTROLLER_H

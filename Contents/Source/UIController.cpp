#include "UIController.h"
#include <cstdlib>
#include <iostream>
void UIController::onClick(const tomato::MouseClickEvent& mouseEvt)
{
	//glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
	std::cout << "successfully exit\n";
	//std::exit(0);
}

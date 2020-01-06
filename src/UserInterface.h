#pragma once
#include "dimg.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "glm/glm.hpp"
#include <GLFW/glfw3.h>
#include <string>

class UI{
public:
	UI();	
	bool initImGui(GLFWwindow* window);
	void ImGuiDraw();
	void ImGuiTerminate();
	static int listbox_item_current, listbox_sobel_gradient;
	static int kernelwidth, kernelheight;
	static bool b_squareMatrix;
private:
	
};

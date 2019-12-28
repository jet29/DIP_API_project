#pragma once
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
private:
	
};

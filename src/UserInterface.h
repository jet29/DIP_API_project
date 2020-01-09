#pragma once
#include "dimg.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "glm/glm.hpp"
#include <GLFW/glfw3.h>
#include <string>
#include <stb_image.h>

class UI{
public:

	static int listbox_item_current, listbox_sobel_gradient;
	static int kernelwidth, kernelheight;
	static bool b_squareMatrix;
	static float LoG_scale;
private:
	GLuint histogram;
public:
	UI();
	bool initImGui(GLFWwindow* window);
	void ImGuiDraw();
	void ImGuiTerminate();
	void setHistogram(GLuint histogram);
};

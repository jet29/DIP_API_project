#include "UserInterface.h"

int UI::listbox_item_current = 0;
int UI::listbox_sobel_gradient = 0;
int UI::kernelheight = 3;
int UI::kernelwidth  = 3;
bool UI::b_squareMatrix = 1;

UI::UI() {
	histogram = 0;
}

bool UI::initImGui(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	if (!ImGui_ImplGlfw_InitForOpenGL(window, true) || !ImGui_ImplOpenGL3_Init("#version 330 core"))
		return false;
	ImGui::StyleColorsDark();

	return true;
}

void UI::ImGuiDraw() {
	// Start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Set ImGui parameters
	// Techniques list
	ImGui::Begin("Image settings");
	const char* items[] = { "Regular","Negative","Grayscale", "Black and White","Sobel","Roberts","Prewitt",
						    "Mean","Median","Gaussian Laplace"};

	ImGui::Combo("Techniques\n",&listbox_item_current,items,IM_ARRAYSIZE(items));

	// Kernel size menu
	int w_min=2, w_max=7, h_min=2, h_max=7;
	if (listbox_item_current > 3) {
		ImGui::Separator();
		if (ImGui::CollapsingHeader("Kernel Size", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (b_squareMatrix == 0){
				w_min = kernelheight == 1 ? w_min = 2 : w_min = 1;
				h_min = kernelwidth == 1 ? h_min = 2 : h_min = 1;
				ImGui::SliderInt("Width", &kernelwidth, w_min, w_max);
				ImGui::SliderInt("Height", &kernelheight, h_min, h_max);
			}
			else {
				w_min = h_min = 2;
				ImGui::SliderInt("Size", &kernelwidth, h_min, h_max);
				kernelheight = kernelwidth;
			}
			ImGui::Checkbox("Square Matrix", &b_squareMatrix);
		}
	}

	ImGui::Separator();
	if (ImGui::CollapsingHeader("Histogram", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::Image((void*)histogram, ImVec2(312, 150));
	}
	ImGui::End();

	// Render dear imgui into screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::ImGuiTerminate() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void UI::setHistogram(GLuint histogram){
	this->histogram = histogram;
}
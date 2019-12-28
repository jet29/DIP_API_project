#pragma once

#include "AntTweakBar.h"
#include "glm/glm.hpp"
#include <GLFW/glfw3.h>
#include <string>

//Singleton user interface class
class UI{
private:
	static UI * interface; //Holds the instance of the class
	TwBar *tbInterface;

public:
	///Method to obtain the only instance of the calls
	static UI * Instance();
	void reshape(GLFWwindow* window, int width, int height);
private:
	///Private constructor
	UI();
	
};

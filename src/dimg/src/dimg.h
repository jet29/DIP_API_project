#pragma once
#include <glad/glad.h> // Glad has to be include before glfw
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <stb_image.h>
#include "../assets/libraries/stb_image_write.h"
#include "../assets/libraries/Shader.h"


class DIMG {
public:

	/**
	*
	*
	*
	**/
	DIMG();

	/**
	*
	*
	*
	**/
	DIMG(int windowWidth, int windowHeight);

	/**
	* DIMG destructor
	**/
	~DIMG();

	/**
	* 
	* 
	* 
	**/
	void test();

	/**
	* Loads a texture into the GPU
	* @param{const char} path of the texture file
	* @returns{unsigned int} GPU texture index
	* */
	unsigned int loadImage(const char *path);

	/**
	* Loads a texture into the GPU
	* @param{const char} path of the texture file FIXXXXXXXXXXXXXXXXXXX
	* @returns{unsigned int} GPU texture index
	* */
	unsigned int negative(GLuint image);

	void setArrayAndBufferObjects(GLuint VAO, GLuint VBO);
private:
	// OPENGL not visible window pointer (Mandatory to be able to create an OpenGL context) 
	GLFWwindow *window;
	// Current viewport resolution
	int windowHeight, windowWidth;
	// VAO, VBO for a fullscreen quad
	GLuint VAO, VBO;
	// Deferred shading texture
	GLuint dsTexture;
	// Framebuffers
	GLuint framebuffer, depthRenderBuffer;

	void init();

	/**
	* Loads a texture into the GPU
	* @param{const char} path of the texture file FIXXXXXXXXXXXXXXXXXXX
	* @returns{unsigned int} GPU texture index
	* */
	bool setFrameBuffer();

	/**
	* Loads a texture into the GPU
	* @param{const char} path of the texture file FIXXXXXXXXXXXXXXXXXXX
	* @returns{unsigned int} GPU texture index
	* */
	void buildScreenQuad(GLuint &VAO, GLuint &VBO);



	/**
	 * Initialize the glfw library
	 * @returns{bool} true if everything goes ok
	 * */
	bool initWindow();

	/**
	 * Initialize the opengl context
	 * */
	void initGL();

	/**
	 * Initialize the glad library
	 * @returns{bool} true if everything goes ok
	 * */
	bool initGlad();

	/**
	 * Handles the window resize
	 * @param{GLFWwindow} window pointer
	 * @param{int} new width of the window
	 * @param{int} new height of the window
	 * */
	void resize(GLFWwindow *window, int width, int height);
};
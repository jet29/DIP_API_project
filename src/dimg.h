#pragma once
#include <glad/glad.h> // Glad has to be include before glfw
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include <iostream>
#include <vector>
#include <string>
#include <Commdlg.h>

enum {
	DIMG_NONE,
	DIMG_COLOR,
	DIMG_NEGATIVE,
	DIMG_GRAYSCALE,
	DIMG_BLACK_AND_WHITE,
	DIMG_SOBEL_GRAD,
	DIMG_ROBERTS_GRAD,
	DIMG_PREWITT_GRAD,
	DIMG_LOG_GRAD,
	DIMG_MEAN_BLUR,
	DIMG_MEDIAN_BLUR,
	DIMG_TOON_SHADING
}typedef DIMG_ENUM;

struct {
	unsigned char *data;
	const char* path;
	int width, height;
	int numOfChannels;
}typedef IMGDATA;

class dimg{
// Public  variables
public:
	unsigned char *inputImage, *outputImage;
// Private variables
private:
	// Working image data
	IMGDATA image;
	// Window current width
	GLuint windowWidth  = 800;
	// Window current height
	GLuint windowHeight = 600;
	// Window pointer
	GLFWwindow* window;
	// Shader object
	Shader* shader;
	// Index (GPU) of the geometry buffer
	unsigned int VBO;
	// Index (GPU) vertex array object
	unsigned int VAO;
	// Index (GPU) of the texture
	unsigned int imageID;
	// Index (GPU) of deferred shading buffers
	unsigned int framebuffer, depthBuffer;
// Public  functions
public:
	dimg();
	void dimg_terminate();
	bool init();
	bool dimg_negative (const char* target, const char* dest, bool hwAcc);
	bool dimg_grayscale(const char* target, const char* dest, bool hwAcc);
	bool dimg_black_and_white(const char* target, const char* dest, int threshold, bool hwAcc);
// Private functions
private:
	bool initWindow();
	bool initGlad();
	void initGL();
	void buildGeometry();
	bool createTexture2D(IMGDATA img, unsigned int& id);
	void terminate_filter(unsigned int& tex1, unsigned int& tex2, IMGDATA &img1, IMGDATA &img2);
	void initImage(IMGDATA &dest, unsigned char* data, int width, int height, int numOfChannels, const char* path);
	bool loadImage(const char* path, IMGDATA& img);
	bool saveImage(IMGDATA img);
	/**
	 * Set OpenGL configuration for a
	 * custom Frame Buffer
	 * */
	bool setFrameBuffer(int width, int height);

	void deferredShading(unsigned int id);
};

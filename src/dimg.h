#pragma once
#include <glad/glad.h> // Glad has to be include before glfw
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include <iostream>
#include <vector>
#include <string>
#include <Commdlg.h>

#define DIMG_HARDWARE_ACCELERATION 0x0001
#define DIMG_CPU_ONLY 0x0002
#define	DIMG_NONE 0x0003
#define	DIMG_COLOR 0x0004
#define	DIMG_NEGATIVE 0x0005
#define	DIMG_GRAYSCALE 0x0006
#define	DIMG_BLACK_AND_WHITE 0x0007
#define	DIMG_SOBEL_EDGE_DETECTION 0x0008
#define	DIMG_ROBERTS_EDGE_DETECTION 0x0009
#define	DIMG_PREWITT_EDGE_DETECTION 0x0010
#define	DIMG_LOG_EDGE_DETECTION 0x0011
#define	DIMG_MEAN_BLUR 0x0012
#define	DIMG_MEDIAN 0x0013
#define	DIMG_TOON_SHADING 0x0014

typedef unsigned int DIMGenum;

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
	bool dimg_negative (const char* target, const char* dest, DIMGenum hwAcc);
	bool dimg_grayscale(const char* target, const char* dest, DIMGenum hwAcc);
	bool dimg_black_and_white(const char* target, const char* dest, int threshold, DIMGenum hwAcc);
	bool dimg_mean_blur(const char* target, const char* dest, int kernelWidth, int kernelHeight, DIMGenum hwAcc);
	bool dimg_median(const char* target, const char* dest, int kernelWidth, int kernelHeight, DIMGenum hwAcc);
	bool dimg_sobel_edge_detection(const char* target, const char* dest, int kernelWidth, int kernelHeight, DIMGenum hwAcc);
	bool dimg_roberts_edge_detection(const char* target, const char* dest, int kernelWidth, int kernelHeight, DIMGenum hwAcc);
	bool dimg_prewitt_edge_detection(const char* target, const char* dest, int kernelWidth, int kernelHeight, DIMGenum hwAcc);
	bool dimg_log_edge_detection(const char* target, const char* dest, int kernelWidth, int kernelHeight,float sigma, DIMGenum hwAcc);
	bool dimg_toon_shading(const char* target, const char* dest, int borderRadius, int colorDiscretization, DIMGenum hwAcc);
	//bool dimg_custom_filter(std::vector<float> kernel);
// Private functions
private:
	bool initWindow();
	bool initGlad();
	void initGL();
	void buildGeometry();
	void setKernel(DIMGenum type, unsigned int &kernel, int kernelWidth, int kernelHeight);
	void setKernel(DIMGenum type, unsigned int& gx, unsigned int& gy, int kernelWidth, int kernelHeight);
	void setKernel(DIMGenum type, unsigned int& gx, float sigma, int kernelWidth, int kernelHeight);
	void setKernel(DIMGenum type, std::vector<float> &kernel, int kernelWidth, int kernelHeight);
	void setKernel(DIMGenum type, std::vector<float> &gx, std::vector<float> &gy, int kernelWidth, int kernelHeight);
	void setKernel(DIMGenum type, std::vector<float> &gx, float sigma, int kernelWidth, int kernelHeight);
	bool isInImage(int x, int y, int width, int height);
	void sort(std::vector<glm::ivec3>& array);
	bool createTexture2D(IMGDATA img, unsigned int& id);
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

#pragma once
#include "Shader.h"
#include <iostream>
#include <glad/glad.h>
#include <vector>
#include <string>
#include <Commdlg.h>

using namespace std;
using std::vector;

#define DIMG_NONE 0x00000
#define DIMG_COLOR 0x00100
#define DIMG_NEGATIVE 0x00101
#define DIMG_GRAYSCALE 0x00102
#define DIMG_BLACKANDWHITE 0x00103
#define DIMG_SOBEL_GRAD 0x00104
#define DIMG_ROBERTS_GRAD 0x00105
#define DIMG_PREWITT_GRAD 0x00106
#define DIMG_MEAN_BLUR 0x00107
#define DIMG_MEDIAN_BLUR 0x00108
#define DIMG_LOG_GRAD 0x00109
#define DIMG_TOON_SHADING 0x00110

#define PI 3.14159265359

class DIMG{
private:
	
	vector<int> kernelData,kData;
	vector<float> gxData, gyData;
	GLuint kernel, gx, gy;
	int currentShader;
	bool flag;
	glm::ivec2 size,k_size;
	glm::ivec2 res;
	int bpp,dpi;
	long uniqueColors;
	unsigned char* imageData;
	GLuint histogram[4], cpuTexture;
	float f_threshold;
	int i_threshold, ctl_threshold;
public:
	float c,cc;
	Shader *shader;
	DIMG();
	~DIMG();
	
	void reloadShader();

	void setKernelSize(int h, int w);

	GLuint getHistogram(int index);

	glm::ivec2 getKernelSize();

	glm::ivec2 getImgResolution();

	int getImageDPI();

	int getImageBPP();

	long getImageUniqueColors();

	void setIntThreshold(int threshold);

	void setFloatThreshold(float threshold);

	void sort(std::vector<glm::ivec3> &array);

	GLuint loadImage(string path = "");

	std::string saveImage(GLuint image);

	/**
	 * Render image
	 * @param{GLuint} destiny image
	 * */
	void color(GLuint image);


	void negative(GLuint image, bool hardwareAcceleration);
	

	void grayscale(GLuint image, bool hardwareAcceleration);

	void blackandwhite(GLuint image, bool hardwareAcceleration);

	/**
	 * Apply sobel border technique
	 * @param{GLuint} destiny image
	 * */
	void sobel(GLuint image, bool hardwareAcceleration);

	void roberts(GLuint image, bool hardwareAcceleration);


	void prewitt(GLuint image, bool hardwareAcceleration);

	
	void mean(GLuint image, bool hardwareAcceleration);


	void median(GLuint image, bool hardwareAcceleration);

	void gaussianLaplace(GLuint image, bool hardwareAcceleration);

	void toonShading(GLuint image, GLuint median, GLuint sobel);

	std::string loadPath();

	std::string savePath();

	void computeHistogram(GLuint image);

private:
	// PRIVATE FUNCTIONS
	void setKernel(bool hardwareAcceleration);
	void computeDPI();
	void computeUniqueColors();
	void createTexture2D(GLuint& target, GLubyte* data, glm::ivec2 size);
	bool isInImage(int x, int y);
};	

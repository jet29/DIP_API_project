#pragma once
#include "Shader.h"
#include <iostream>
#include <glad/glad.h>
#include <vector>
#include <string>
#include <Commdlg.h>

using std::vector;

#define DIMG_COLOR 0x00100
#define DIMG_NEGATIVE 0x00101
#define DIMG_GRAYSCALE 0x00102
#define DIMG_BLACKANDWHITE 0x00103
#define DIMG_SOBEL_GRAD 0x00104
#define DIMG_ROBERTS_GRAD 0x00105
#define DIMG_PREWITT_GRAD 0x00106
#define DIMG_MEAN_BLUR 0x00107
#define DIMG_MEDIAN_BLUR 0x00108
#define DIMG_GAUSSIAN_BLUR 0x00109


class DIMG{
private:
	
	vector<int> mat;
	GLuint matTex;
	int currentShader;
	glm::ivec2 size;
	// PRIVATE FUNCTIONS
	void setMatText();
public:
	Shader *shader;
	DIMG();
	~DIMG();
	/**
	 *
	 * */
	void reloadShader();

	/**
	 *
	 * */
	void setKernelSize(int h, int w);

	/** 
	 * Overloaded function
	 * Loads an image as a texture into the GPU
	 * @returns{unsigned int} GPU texture index
	 * */
	GLuint loadImage();

	/**
	 * Overloaded function
	 * Loads an image as a texture into the GPU
	 * @param{const char} path of the texture file
	 * @returns{unsigned int} GPU texture index
	 * */
	GLuint loadImage(const char* path);
	
	/**
	 *
	 * */
	void saveImage(GLuint image);

	/**
	 *
	 * */
	void negative(GLuint image);
	
	/**
	 *
	 * */
	void grayscale(GLuint image);
	
	/**
	 *
	 * */
	void blackandwhite(GLuint image);

	/**
	 * Apply sobel border technique
	 * @param{GLuint} destiny image
	 * */
	void sobel(GLuint image);

	/**
	 *
	 * */
	void roberts(GLuint image);

	/**
	 *
	 * */
	void prewitt(GLuint image);

	/**
	 *
	 * */
	void mean(GLuint image);

	/**
	 *
	 * */
	void median(GLuint image);

	/**
	 *
	 * */
	void gaussianLaplace(GLuint image);

	/**
	 * Apply sobel border technique
	 * @param{GLuint} destiny image
	 * */
	void color(GLuint image);
	
	/**
	 *
	 * */
	std::string loadPath();

	/**
	 *
	 * */
	std::string savePath();


};	

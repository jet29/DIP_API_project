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
#define DIMG_CUSTOM_KERNEL 0x015
#define DIMG_RED 0x0016
#define DIMG_GREEN 0x0017
#define DIMG_BLUE 0x0018

typedef unsigned int DIMGenum;

struct
{
	unsigned char *data;
	const char *path;
	int width, height;
	int numOfChannels;
} typedef IMGDATA;

class dimg
{
	// Public  variables
public:
	unsigned char *inputImage, *outputImage;
	// Private variables
private:
	// Working image data
	IMGDATA image;
	// Window current width
	GLuint windowWidth = 800;
	// Window current height
	GLuint windowHeight = 600;
	// Window pointer
	GLFWwindow *window;
	// Shader object
	Shader *shader;
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


	/**
	 * @brief Construct a new dimg object
	 * 
	 */
	dimg();

	/**
	 * @brief finish dimg instance
	 * 
	 */
	void dimg_terminate();

	/**
	 * @brief init dimg instance. this function must be called before using any API function
	 * 
	 * @return true if success
	 * @return false if failed 
	 */
	bool init();


	/**
	 * @brief compute the negative of an image 
	 * 
	 * @param target route of the image
	 * @param dest directory of the result image
	 * @param hwAcc use hardware acceleration or not
	 * @return true if success
	 * @return false if failed
	 */
	bool dimg_negative(const char *target, const char *dest, DIMGenum hwAcc);


	/**
	 * @brief compute the grayscale of an image 
	 * 
	 * @param target route of the image
	 * @param dest directory of the result image
	 * @param hwAcc use hardware acceleration or not
	 * @return true if success
	 * @return false if failed
	 */
	bool dimg_grayscale(const char *target, const char *dest, DIMGenum hwAcc);

	/**
	 * @brief compute the black and white of an image 
	 * 
	 * @param target route of the image
	 * @param dest directory of the result image
	 * @param threshhold limit for the grayscale value to be black or white
	 * @param hwAcc use hardware acceleration or not
	 * @return true if success
	 * @return false if failed
	 */
	bool dimg_black_and_white(const char *target, const char *dest, int threshold, DIMGenum hwAcc);


	/**
	 * @brief compute the Mean filter of an image 
	 * 
	 * @param target route of the image
	 * @param dest directory of the result image
	 * @param kernelWidth width size of the kernel
	 * @param kernelHeight height size of the kernel
	 * @param hwAcc use hardware acceleration or not
	 * @return true if success
	 * @return false if failed
	 */
	bool dimg_mean_blur(const char *target, const char *dest, int kernelWidth, int kernelHeight, DIMGenum hwAcc);
	
	/**
	 * @brief compute the Median filter of an image 
	 * 
	 * @param target route of the image
	 * @param dest directory of the result image
	 * @param kernelWidth width size of the kernel
	 * @param kernelHeight height size of the kernel
	 * @param hwAcc use hardware acceleration or not
	 * @return true if success
	 * @return false if failed
	 */
	bool dimg_median(const char *target, const char *dest, int kernelWidth, int kernelHeight, DIMGenum hwAcc);
	
	/**
	 * @brief compute the Sobel filter of an image 
	 * 
	 * @param target route of the image
	 * @param dest directory of the result image
	 * @param kernelWidth width size of the kernel
	 * @param kernelHeight height size of the kernel
	 * @param hwAcc use hardware acceleration or not
	 * @return true if success
	 * @return false if failed
	 */
	bool dimg_sobel_edge_detection(const char *target, const char *dest, int kernelWidth, int kernelHeight, DIMGenum hwAcc);
	
	/**
	 * @brief compute the Roberts filter of an image 
	 * 
	 * @param target route of the image
	 * @param dest directory of the result image
	 * @param kernelWidth width size of the kernel
	 * @param kernelHeight height size of the kernel
	 * @param hwAcc use hardware acceleration or not
	 * @return true if success
	 * @return false if failed
	 */
	bool dimg_roberts_edge_detection(const char *target, const char *dest, int kernelWidth, int kernelHeight, DIMGenum hwAcc);
	
	/**
	 * @brief compute the Prewitt filter of an image 
	 * 
	 * @param target route of the image
	 * @param dest directory of the result image
	 * @param kernelWidth width size of the kernel
	 * @param kernelHeight height size of the kernel
	 * @param hwAcc use hardware acceleration or not
	 * @return true if success
	 * @return false if failed
	 */
	bool dimg_prewitt_edge_detection(const char *target, const char *dest, int kernelWidth, int kernelHeight, DIMGenum hwAcc);
	
	/**
	 * @brief compute the LoG filter of an image 
	 * 
	 * @param target route of the image
	 * @param dest directory of the result image
	 * @param kernelWidth width size of the kernel
	 * @param kernelHeight height size of the kernel
	 * @param sigma alter the result of LoG
	 * @param hwAcc use hardware acceleration or not
	 * @return true if success
	 * @return false if failed
	 */
	bool dimg_log_edge_detection(const char *target, const char *dest, int kernelWidth, int kernelHeight, float sigma, DIMGenum hwAcc);
	
	
	/**
	 * @brief compute the toon shading filter of an image 
	 * 
	 * @param target route of the image
	 * @param dest directory of the result image
	 * @param borderRadius sobel border radius
	 * @param colorDiscretization discretization level of median filter
	 * @param hwAcc use hardware acceleration or not
	 * @return true if success
	 * @return false if failed
	 */
	bool dimg_toon_shading(const char *target, const char *dest, int borderRadius, int colorDiscretization, DIMGenum hwAcc);
	
	/**
	 * @brief compute the histogram of an image
	 * 
	 * @param target route of the image
	 * @param dest directory of the result image
	 * @param width width size of the histogram
	 * @param height height size of the histogram
	 * @param type DIMG_RED for red channel histogram, DIMG_GREEN for green channel histogram and DIMG_BLUE for blue channel histogram
	 * @return true if success
	 * @return false if failed
	 */
	bool dimg_histogram(const char *target, const char *dest, int width, int height, DIMGenum type);
	
	/**
	 * @brief compute a custom filter giving a matrix
	 * 
	 * @param target route of the image
	 * @param dest directory of the result image
	 * @param kernelWidth width size of the kernel
	 * @param kernelHeight height size of the kernel
	 * @param kernelMatrix
	 * @param hwAcc use hardware acceleration or not
	 * @return true if success
	 * @return false if failed
	 */
	bool dimg_custom_filter(const char* target, const char* dest, int kernelWidth, int kernelHeight, float* kernelMatrix, DIMGenum hwAcc);


	/**
	 * @brief compute unique color for an image
	 * 
	 * @param target image route
	 * @param unique_colors destiny variable passed by reference
	 * @return true if success
	 * @return false if failed
	 */
	bool dimg_unique_colors(const char* target, int& unique_colors);

	/**
	 * @brief compute image dimension for an image
	 * 
	 * @param target image route
	 * @param width destiny variable for width passed by reference
	 * @param height destiny variable for height passed by reference
	 * @return true if success
	 * @return false if failed
	 */
	bool dimg_image_dimension(const char* target, int& width, int& height);

	/**
	 * @brief compute image bits per pixel
	 * 
	 * @param target image route
	 * @param bpp destiny variable passed by reference
	 * @return true if success
	 * @return false if failed
	 */
	bool dimg_image_bpp(const char* target, int& bpp);

	/**
	 * @brief compute image dots per inch
	 * 
	 * @param target image route
	 * @param dpi destiny variable passed by reference
	 * @return true if success
	 * @return false if failed
	 */
	bool dimg_image_dpi(const char* target, int& dpi);


private:
	bool initWindow();
	bool initGlad();
	void initGL();
	void buildGeometry();
	void setKernel(DIMGenum type, unsigned int &kernel, int kernelWidth, int kernelHeight);
	void setKernel(DIMGenum type, unsigned int& gx, unsigned int& gy, int kernelWidth, int kernelHeight);
	void setKernel(DIMGenum type, unsigned int& gx, float sigma, int kernelWidth, int kernelHeight);
	void setKernel(DIMGenum type, unsigned int& kernel, float* kernelMatrix, int kernelWidth, int kernelHeight);
	void setKernel(DIMGenum type, std::vector<float> &kernel, int kernelWidth, int kernelHeight);
	void setKernel(DIMGenum type, std::vector<float> &gx, std::vector<float> &gy, int kernelWidth, int kernelHeight);
	void setKernel(DIMGenum type, std::vector<float> &gx, float sigma, int kernelWidth, int kernelHeight);
	bool isInImage(int x, int y, int width, int height);
	void sort(std::vector<glm::ivec3> &array);
	bool createTexture2D(IMGDATA img, unsigned int &id);
	void initImage(IMGDATA &dest, unsigned char *data, int width, int height, int numOfChannels, const char *path);
	bool loadImage(const char *path, IMGDATA &img);
	bool saveImage(IMGDATA img, bool flipVertical);
	/**
	 * Set OpenGL configuration for a
	 * custom Frame Buffer
	 * */
	bool setFrameBuffer(int width, int height);

	void deferredShading(unsigned int id);
};

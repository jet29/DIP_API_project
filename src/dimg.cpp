#include "DIMG.h"
#include <stb_image.h>
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>

using namespace std;

DIMG::DIMG(){
	// Initialize matrix
	kernelData = vector<int>(49, 0);
	// Default 3x3 Kernel's size
	size = glm::ivec2(3);
	k_size = size;
	currentShader = DIMG_NONE;
	c = 1.0f;
	cc = c;
	res = glm::ivec2(0, 0);
	bpp = 0;
	dpi = 0;
}

DIMG::~DIMG(){
}

bool DIMG::isInImage(int x, int y) {
	return x < 0 || x >= (res.y-1)*res.x*3 || y < 0 || y >= res.x*3 ? false : true;
}

void DIMG::reloadShader() {
	delete shader;
	switch (currentShader) {
	case DIMG_COLOR:
		shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
		break;
	case DIMG_NEGATIVE:
		shader = new Shader("assets/shaders/negative.vert", "assets/shaders/negative.frag");
		break;
	case DIMG_GRAYSCALE:
		shader = new Shader("assets/shaders/grayscale.vert", "assets/shaders/grayscale.frag");
		break;
	case DIMG_BLACKANDWHITE:
		shader = new Shader("assets/shaders/blackandwhite.vert", "assets/shaders/blackandwhite.frag");
		break;
	case DIMG_SOBEL_GRAD:
		shader = new Shader("assets/shaders/sobel.vert", "assets/shaders/sobel.frag");
		break;
	case DIMG_ROBERTS_GRAD:
		shader = new Shader("assets/shaders/roberts.vert", "assets/shaders/roberts.frag");
		break;
	case DIMG_PREWITT_GRAD:
		shader = new Shader("assets/shaders/prewitt.vert", "assets/shaders/prewitt.frag");
		break;
	case DIMG_MEAN_BLUR:
		shader = new Shader("assets/shaders/mean.vert", "assets/shaders/mean.frag");
		break;
	case DIMG_MEDIAN_BLUR:
		shader = new Shader("assets/shaders/median.vert", "assets/shaders/median.frag");
		break;
	case DIMG_LOG_GRAD:
		shader = new Shader("assets/shaders/gaussian.vert", "assets/shaders/gaussian.frag");
		break;
	}
}

void DIMG::setKernelSize(int h, int w) {
	size = glm::ivec2(h, w);
}

glm::ivec2 DIMG::getKernelSize() {
	return size;
}

glm::ivec2 DIMG::getImgResolution() {
	return res;
}

int DIMG::getImageBPP() {
	return bpp;
}

int DIMG::getImageDPI() {
	return dpi;
}

void DIMG::computeDPI() {
	dpi = (int)glm::round((float)res.x * 25.4 / (float)(res.x * 0.264583));
}

long DIMG::getImageUniqueColors() {
	return uniqueColors;
}

void DIMG::setIntThreshold(int threshold) {
	i_threshold = threshold;
}

void DIMG::setFloatThreshold(float threshold) {
	f_threshold = threshold;
}

void DIMG::sort(std::vector<glm::ivec3> &array){
	glm::vec3 temp;
	for (unsigned int i = 0; i < array.size(); i++)
		for (unsigned int j = 0; j < array.size(); j++)
			if (array[i].r > array[j].r) {
				temp = array[j];
				array[i] = array[j];
				array[j] = temp;
			}
}

GLuint DIMG::getHistogram(int index) {
	index = index < 0 ? 0 : index;
	return histogram[index];
}

int _3DRGBto1DRGB(int x, int y, int z) {
	return (255 * 255 * z) + (255 * y) + x;
}

void DIMG::computeUniqueColors() {
	std::vector<char> color = std::vector<char>(256 * 256 * 256, 0);
	int totalBytes = res.x * res.y * 3;
	int pos,x,y,z;
	for (int i = 0; i < totalBytes; i+=3) {
		x = imageData[i];
		y = imageData[i + 1];
		z = imageData[i + 2];
		pos = _3DRGBto1DRGB(x, y, z);
		if (color[pos] == 0) {
			color[pos] = 1;
			uniqueColors++;
		}
	}
}

void DIMG::createTexture2D(GLuint &target, GLubyte* data, glm::ivec2 size){
	// Binds the texture
	glBindTexture(GL_TEXTURE_2D, target);
	// Creates the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, res.x, res.y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	// Creates the texture mipmaps
	glGenerateMipmap(GL_TEXTURE_2D);

	// Set the filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenTextures(1, &target);
	glBindTexture(GL_TEXTURE_2D, target);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint DIMG::loadImage(){
	// Using Windows OS primitive to get retrieve paths
	std::string winPath = loadPath();
	const char *path = winPath.c_str();
	unsigned int id;
	// Creates the texture on GPU
	glGenTextures(1, &id);
	// Loads the texture
	int textureWidth, textureHeight, numberOfChannels;
	// Flips the texture when loads it because in opengl the texture coordinates are flipped
	stbi_set_flip_vertically_on_load(true);
	// Loads the texture file data
	unsigned char *data = stbi_load(path, &textureWidth, &textureHeight, &numberOfChannels, 0);
	if (data)
	{
		// Gets the texture channel format
		GLenum format;
		switch (numberOfChannels)
		{
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		}

		// Binds the texture
		glBindTexture(GL_TEXTURE_2D, id);
		// Creates the texture
		glTexImage2D(GL_TEXTURE_2D, 0, format, textureWidth, textureHeight, 0, format, GL_UNSIGNED_BYTE, data);
		// Creates the texture mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);

		// Set the filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "ERROR:: Unable to load texture " << path << std::endl;
		glDeleteTextures(1, &id);
	}
	// We dont need the data texture anymore because is loaded on the GPU
	stbi_image_free(data);

	return id;
}

GLuint DIMG::loadImage(const char* path) {
	unsigned int id;
	// Creates the texture on GPU
	glGenTextures(1, &id);
	// Flips the texture when loads it because in opengl the texture coordinates are flipped
	stbi_set_flip_vertically_on_load(true);
	// Loads the texture file data
	imageData = stbi_load(path, &res.x, &res.y, &bpp, 0);
	if (imageData)
	{
		// Gets the texture channel format
		GLenum format;
		switch (bpp)
		{
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		}
		bpp = (int) pow(2, bpp);
		// Binds the texture
		glBindTexture(GL_TEXTURE_2D, id);
		// Creates the texture
		glTexImage2D(GL_TEXTURE_2D, 0, format, res.x, res.y, 0, format, GL_UNSIGNED_BYTE, imageData);
		// Creates the texture mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);

		// Set the filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "ERROR:: Unable to load texture " << path << std::endl;
		glDeleteTextures(1, &id);
	}
	//// We dont need the data texture anymore because is loaded on the GPU
	//stbi_image_free(imageData);
	// Compute histogram
	computeHistogram(id);
	computeDPI();
	computeUniqueColors();
	return id;
}

std::string DIMG::saveImage(GLuint image) {
	GLubyte* pixels = new GLubyte[800 * 600 * 3];
	glBindTexture(GL_TEXTURE_2D, image);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	// You have to use 3 comp for complete jpg file. If not, the image will be grayscale or nothing.
	stbi_flip_vertically_on_write(true); // flag is non-zero to flip data vertically
	std::string path = savePath();
	stbi_write_jpg(path.c_str(), 800, 600, 3, pixels, 100);

	return path;
}

void DIMG::color(GLuint image) {
	shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
	currentShader = DIMG_COLOR;
	shader->use();
	// Send image to GPU
	shader->setInt("image", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image);
}

void DIMG::negative(GLuint image, bool hardwareAcceleration){
	if (hardwareAcceleration) {
		shader = new Shader("assets/shaders/negative.vert", "assets/shaders/negative.frag");
	}
	else {
		if (currentShader != DIMG_NEGATIVE) {
			unsigned char* negative = new unsigned char[res.x * res.y * 3];
			memcpy(negative, imageData, res.x * res.y * 3);
			for (int i = 0; i < res.x*res.y*3; i++)
				negative[i] = 255 - imageData[i];
			createTexture2D(cpuTexture, negative, res);
		}
		shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
		image = cpuTexture;
	}
	currentShader = DIMG_NEGATIVE;
	shader->use();
	// Send image to GPU
	shader->setInt("image", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image);
}

void DIMG::grayscale(GLuint image, bool hardwareAcceleration) {
	if (hardwareAcceleration)
		shader = new Shader("assets/shaders/grayscale.vert", "assets/shaders/grayscale.frag");
	else {
		if (currentShader != DIMG_GRAYSCALE) {
			unsigned char* grayscale = new unsigned char[res.x * res.y * 3] ;
			memcpy(grayscale, imageData, res.x * res.y * 3);
			int gs_value;
			for (int i = 0; i < res.x * res.y * 3 - 3; i += 3) {
				gs_value = (int)(imageData[i] * 0.2989) + 
						   (int)(imageData[i + 1] * 0.5870) +
						   (int)(imageData[i + 2] * 0.1140);
				// Red
				grayscale[i] = gs_value;
				// Green
				grayscale[i + 1] = gs_value;
				// Blue
				grayscale[i + 2] = gs_value;
			}
			createTexture2D(cpuTexture, grayscale, res);
		}
		shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
		image = cpuTexture;
	}
	currentShader = DIMG_GRAYSCALE;
	shader->use();
	// Send image to GPU
	shader->setInt("image", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image);
}

void DIMG::blackandwhite(GLuint image, bool hardwareAcceleration) {
	if (hardwareAcceleration)
		shader = new Shader("assets/shaders/blackandwhite.vert", "assets/shaders/blackandwhite.frag");
	else {
		if (currentShader != DIMG_BLACKANDWHITE || ctl_threshold != i_threshold ) {
			unsigned char* blackandwhite = new unsigned char[res.x * res.y * 3];
			memcpy(blackandwhite, imageData, res.x * res.y * 3);
			int gs_value, bnw;
			ctl_threshold = i_threshold;
			for (int i = 0; i < res.x * res.y * 3 - 3; i += 3) {
				gs_value = (int)(imageData[i] * 0.2989) +
					(int)(imageData[i + 1] * 0.5870) +
					(int)(imageData[i + 2] * 0.1140);
				bnw = gs_value > i_threshold ? 255 : 0;
				// Red
				blackandwhite[i]     = bnw;
				// Green
				blackandwhite[i + 1] = bnw;
				// Blue
				blackandwhite[i + 2] = bnw;
			}
			createTexture2D(cpuTexture, blackandwhite, res);
		}
		shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
		image = cpuTexture;
	}
	currentShader = DIMG_BLACKANDWHITE;
	shader->use();
	// Send image to GPU
	shader->setInt("image", 0);
	shader->setFloat("f_threshold", f_threshold);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image);
}

void DIMG::sobel(GLuint image, bool hardwareAcceleration){
	if (hardwareAcceleration) {
		if (currentShader != DIMG_SOBEL_GRAD || k_size != size) {
			currentShader = DIMG_SOBEL_GRAD;
			setKernel(hardwareAcceleration);
			k_size = size;
			shader = new Shader("assets/shaders/sobel.vert", "assets/shaders/sobel.frag");
		}
	}
	else {
		if (currentShader != DIMG_SOBEL_GRAD || k_size != size) {
			currentShader = DIMG_SOBEL_GRAD;
			setKernel(hardwareAcceleration);
			k_size = size;
			unsigned char* output = new unsigned char[res.x * res.y * 3];
			memset(output, 0, res.x * res.y * 3);
			glm::ivec3 sobel;
			glm::vec3  s_gx, s_gy;
			glm::ivec2 pivot = glm::ivec2(k_size.x / 2, k_size.y / 2);
			glm::ivec2 img_pos, k_pos;
			int cols, rgb_offset, totalSize, gs_value;
			totalSize = size.x * size.y;
			cols = res.x * 3;
			// Image Loop
			for (int i = 0; i < res.y; i++) {
				for (int j = 0; j < cols; j += 3) {
					sobel = glm::ivec3(0);
					s_gx  = glm::ivec3(0);
					s_gy  = glm::ivec3(0);
					img_pos = glm::ivec2(i * cols, j);
					// Kernel Loop
					for (int ii = 0; ii < k_size.x; ii++) {
						//printf("--------------------- KROW: %i\n", ii);
						for (int jj = 0; jj < k_size.y; jj++) {
							rgb_offset = (jj - pivot.y) * 3;
							k_pos = glm::ivec2(img_pos.x + (ii - pivot.x) * cols, img_pos.y + rgb_offset);
							if (!isInImage(k_pos.x, k_pos.y))
								continue;
							/*					printf("[img]rgb(%i,%i,%i)\n",
													imageData[k_pos.x + (k_pos.y + 0)],
													imageData[k_pos.x + (k_pos.y + 1)],
													imageData[k_pos.x + (k_pos.y + 2)]);*/
							// X-axis Gradient
								// red
								s_gx.r += imageData[k_pos.x + (k_pos.y + 0)] * gxData[ii * 7 + jj];
								// green
								s_gx.g += imageData[k_pos.x + (k_pos.y + 1)] * gxData[ii * 7 + jj];
								// blue
								s_gx.b += imageData[k_pos.x + (k_pos.y + 2)] * gxData[ii * 7 + jj];
							// Y-axis Gradient
								// red
								s_gy.r += imageData[k_pos.x + (k_pos.y + 0)] * gyData[ii * 7 + jj];
								// green
								s_gy.g += imageData[k_pos.x + (k_pos.y + 1)] * gyData[ii * 7 + jj];
								// blue
								s_gy.b += imageData[k_pos.x + (k_pos.y + 2)] * gyData[ii * 7 + jj];
						}
						//printf("\n");
					}
					sobel = glm::round(glm::sqrt(s_gx * s_gx + s_gy * s_gy));
					gs_value = (int)(sobel.r * 0.2989) +
							   (int)(sobel.g * 0.5870) +
							   (int)(sobel.b * 0.1140);
					sobel = glm::ivec3(gs_value, gs_value, gs_value);
					// Edge detection final per pixel color
					output[i * cols + (j + 0)] = sobel.r;
					output[i * cols + (j + 1)] = sobel.g;
					output[i * cols + (j + 2)] = sobel.b;
				}
			}
			createTexture2D(cpuTexture, output, res);
		}
		shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
		image = cpuTexture;
	}
	
	shader->use();
	// Send image to GPU
	shader->setInt("image", 0);
	shader->setInt("gx", 1);
	shader->setInt("gy", 2);
	shader->setInt("kWidth", size.x);
	shader->setInt("kHeight", size.y);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, gx);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_1D, gy);
}

void DIMG::roberts(GLuint image, bool hardwareAcceleration) {
	if (hardwareAcceleration) {
		if (currentShader != DIMG_ROBERTS_GRAD || k_size != size) {
			currentShader = DIMG_ROBERTS_GRAD;
			setKernel(hardwareAcceleration);
			k_size = size;
			shader = new Shader("assets/shaders/roberts.vert", "assets/shaders/roberts.frag");
		}
	}
	else {
		if (currentShader != DIMG_ROBERTS_GRAD || k_size != size) {
			currentShader = DIMG_ROBERTS_GRAD;
			setKernel(hardwareAcceleration);
			k_size = size;
			unsigned char* output = new unsigned char[res.x * res.y * 3];
			memset(output, 0, res.x * res.y * 3);
			glm::ivec3 roberts;
			glm::vec3  r_gx, r_gy;
			glm::ivec2 pivot = glm::ivec2(k_size.x / 2, k_size.y / 2);
			glm::ivec2 img_pos, k_pos;
			int cols, rgb_offset, totalSize, gs_value;
			totalSize = size.x * size.y;
			cols = res.x * 3;
			// Image Loop
			for (int i = 0; i < res.y; i++) {
				for (int j = 0; j < cols; j += 3) {
					roberts = glm::ivec3(0);
					r_gx = glm::ivec3(0);
					r_gy = glm::ivec3(0);
					img_pos = glm::ivec2(i * cols, j);
					// Kernel Loop
					for (int ii = 0; ii < k_size.x; ii++) {
						//printf("--------------------- KROW: %i\n", ii);
						for (int jj = 0; jj < k_size.y; jj++) {
							rgb_offset = (jj - pivot.y) * 3;
							k_pos = glm::ivec2(img_pos.x + (ii - pivot.x) * cols, img_pos.y + rgb_offset);
							if (!isInImage(k_pos.x, k_pos.y))
								continue;
							/*					printf("[img]rgb(%i,%i,%i)\n",
													imageData[k_pos.x + (k_pos.y + 0)],
													imageData[k_pos.x + (k_pos.y + 1)],
													imageData[k_pos.x + (k_pos.y + 2)]);*/
													// X-axis Gradient
														// red
							r_gx.r += imageData[k_pos.x + (k_pos.y + 0)] * gxData[ii * 7 + jj];
							// green
							r_gx.g += imageData[k_pos.x + (k_pos.y + 1)] * gxData[ii * 7 + jj];
							// blue
							r_gx.b += imageData[k_pos.x + (k_pos.y + 2)] * gxData[ii * 7 + jj];
							// Y-axis Gradient
								// red
							r_gy.r += imageData[k_pos.x + (k_pos.y + 0)] * gyData[ii * 7 + jj];
							// green
							r_gy.g += imageData[k_pos.x + (k_pos.y + 1)] * gyData[ii * 7 + jj];
							// blue
							r_gy.b += imageData[k_pos.x + (k_pos.y + 2)] * gyData[ii * 7 + jj];
						}
						//printf("\n");
					}
					roberts = glm::round(glm::sqrt(r_gx * r_gx + r_gy * r_gy));
					gs_value = (int)(roberts.r * 0.2989) +
						(int)(roberts.g * 0.5870) +
						(int)(roberts.b * 0.1140);
					roberts = glm::ivec3(gs_value, gs_value, gs_value);
					// Edge detection final per pixel color
					output[i * cols + (j + 0)] = roberts.r;
					output[i * cols + (j + 1)] = roberts.g;
					output[i * cols + (j + 2)] = roberts.b;
				}
			}
			createTexture2D(cpuTexture, output, res);
		}
		shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
		image = cpuTexture;
	}
	shader->use();
	// Send image to GPU
	shader->setInt("image", 0);
	shader->setInt("gx", 1);
	shader->setInt("gy", 2);
	shader->setInt("kWidth", size.x);
	shader->setInt("kHeight", size.y);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, gx);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_1D, gy);
}

void DIMG::prewitt(GLuint image, bool hardwareAcceleration) {
	if (hardwareAcceleration) {
		if (currentShader != DIMG_PREWITT_GRAD || k_size != size) {
			currentShader = DIMG_PREWITT_GRAD;
			setKernel(hardwareAcceleration);
			k_size = size;
			shader = new Shader("assets/shaders/prewitt.vert", "assets/shaders/prewitt.frag");
		}
	}
	else {
		if (currentShader != DIMG_PREWITT_GRAD || k_size != size) {
			currentShader = DIMG_PREWITT_GRAD;
			setKernel(hardwareAcceleration);
			k_size = size;
			unsigned char* output = new unsigned char[res.x * res.y * 3];
			memset(output, 0, res.x * res.y * 3);
			glm::ivec3 prewitt;
			glm::vec3  p_gx, p_gy;
			glm::ivec2 pivot = glm::ivec2(k_size.x / 2, k_size.y / 2);
			glm::ivec2 img_pos, k_pos;
			int cols, rgb_offset, totalSize, gs_value;
			totalSize = size.x * size.y;
			cols = res.x * 3;
			// Image Loop
			for (int i = 0; i < res.y; i++) {
				for (int j = 0; j < cols; j += 3) {
					prewitt = glm::ivec3(0);
					p_gx = glm::ivec3(0);
					p_gy = glm::ivec3(0);
					img_pos = glm::ivec2(i * cols, j);
					// Kernel Loop
					for (int ii = 0; ii < k_size.x; ii++) {
						//printf("--------------------- KROW: %i\n", ii);
						for (int jj = 0; jj < k_size.y; jj++) {
							rgb_offset = (jj - pivot.y) * 3;
							k_pos = glm::ivec2(img_pos.x + (ii - pivot.x) * cols, img_pos.y + rgb_offset);
							if (!isInImage(k_pos.x, k_pos.y))
								continue;
							/*					printf("[img]rgb(%i,%i,%i)\n",
													imageData[k_pos.x + (k_pos.y + 0)],
													imageData[k_pos.x + (k_pos.y + 1)],
													imageData[k_pos.x + (k_pos.y + 2)]);*/
													// X-axis Gradient
														// red
							p_gx.r += imageData[k_pos.x + (k_pos.y + 0)] * gxData[ii * 7 + jj];
							// green
							p_gx.g += imageData[k_pos.x + (k_pos.y + 1)] * gxData[ii * 7 + jj];
							// blue
							p_gx.b += imageData[k_pos.x + (k_pos.y + 2)] * gxData[ii * 7 + jj];
							// Y-axis Gradient
								// red
							p_gy.r += imageData[k_pos.x + (k_pos.y + 0)] * gyData[ii * 7 + jj];
							// green
							p_gy.g += imageData[k_pos.x + (k_pos.y + 1)] * gyData[ii * 7 + jj];
							// blue
							p_gy.b += imageData[k_pos.x + (k_pos.y + 2)] * gyData[ii * 7 + jj];
						}
						//printf("\n");
					}
					prewitt = glm::round(glm::sqrt(p_gx * p_gx + p_gy * p_gy));
					gs_value = (int)(prewitt.r * 0.2989) +
						(int)(prewitt.g * 0.5870) +
						(int)(prewitt.b * 0.1140);
					prewitt = glm::ivec3(gs_value, gs_value, gs_value);
					// Edge detection final per pixel color
					output[i * cols + (j + 0)] = prewitt.r;
					output[i * cols + (j + 1)] = prewitt.g;
					output[i * cols + (j + 2)] = prewitt.b;
				}
			}
			createTexture2D(cpuTexture, output, res);
		}
		shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
		image = cpuTexture;
	}
	shader->use();
	// Send image to GPU
	shader->setInt("image", 0);
	shader->setInt("gx", 1);
	shader->setInt("gy", 2);
	shader->setInt("kWidth", size.x);
	shader->setInt("kHeight", size.y);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, gx);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_1D, gy);
}

void DIMG::mean(GLuint image, bool hardwareAcceleration) {
	if (hardwareAcceleration) {
		if (currentShader != DIMG_MEAN_BLUR || k_size != size) {
			currentShader = DIMG_MEAN_BLUR;
			setKernel(hardwareAcceleration);
			k_size = size;
			shader = new Shader("assets/shaders/mean.vert", "assets/shaders/mean.frag");
		}
	}
	else{
		if (currentShader != DIMG_MEAN_BLUR || k_size != size) {
			currentShader = DIMG_MEAN_BLUR;
			setKernel(hardwareAcceleration);
			k_size = size;
			unsigned char* output = new unsigned char[res.x * res.y * 3];
			memset(output, 0, res.x * res.y * 3);
			glm::ivec3 mean;
			glm::ivec2 pivot = glm::ivec2(k_size.x / 2, k_size.y / 2);
			glm::ivec2 img_pos, k_pos;
			int cols, rgb_offset, totalSize;
			totalSize = size.x * size.y;
			cols = res.x * 3;
			// Image Loop
			for (int i = 0; i < res.y; i++) {
				for (int j = 0; j < cols; j+=3){
					mean = glm::ivec3(0);
					img_pos = glm::ivec2(i * cols, j);
					// Kernel Loop
					for (int ii = 0; ii < k_size.x; ii++) {
						//printf("--------------------- KROW: %i\n", ii);
						for (int jj = 0; jj < k_size.y; jj++) {
							rgb_offset = (jj - pivot.y) * 3;
							k_pos = glm::ivec2(img_pos.x + (ii - pivot.x)*cols, img_pos.y + rgb_offset);
							if (!isInImage(k_pos.x, k_pos.y))
								continue;
		/*					printf("[img]rgb(%i,%i,%i)\n",
								imageData[k_pos.x + (k_pos.y + 0)],
								imageData[k_pos.x + (k_pos.y + 1)],
								imageData[k_pos.x + (k_pos.y + 2)]);*/
							// red
							mean.r += imageData[k_pos.x + (k_pos.y + 0)] * kernelData[ii * 7 + jj];
							// green
							mean.g += imageData[k_pos.x + (k_pos.y + 1)] * kernelData[ii * 7 + jj];
							// blue
							mean.b += imageData[k_pos.x + (k_pos.y + 2)] * kernelData[ii * 7 + jj];
						}
						//printf("\n");
					}
					// Average color
					mean.r = mean.r / totalSize;
					mean.g = mean.g / totalSize;
					mean.b = mean.b / totalSize;
					output[i * cols + (j + 0)] = mean.r;
					output[i * cols + (j + 1)] = mean.g;
					output[i * cols + (j + 2)] = mean.b;
				}
			}
			createTexture2D(cpuTexture, output, res);
		}
		shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
		image = cpuTexture;
	}

		shader->use();
		// Send image to GPU
		shader->setInt("image", 0);
		shader->setInt("kernel", 1);
		shader->setInt("kWidth", size.x);
		shader->setInt("kHeight", size.y);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, image);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_1D, kernel);
}

void DIMG::median(GLuint image, bool hardwareAcceleration) {
	if (hardwareAcceleration) {
		if (currentShader != DIMG_MEDIAN_BLUR || k_size != size) {
			
			k_size = size;
			shader = new Shader("assets/shaders/median.vert", "assets/shaders/median.frag");
		}
	}
	else {
		if (currentShader != DIMG_MEDIAN_BLUR || k_size != size){
			// History kernel size for control purposes
			k_size = size;
			// Output image
			unsigned char* output = new unsigned char[res.x * res.y * 3];
			memset(output, 0, res.x * res.y * 3);
			// Median CPU computation 
			std::vector<glm::ivec3> rgb_array;
			glm::ivec2 pivot = glm::ivec2(k_size.x / 2, k_size.y / 2);
			glm::ivec2 img_pos, k_pos;
			int cols, rgb_offset, totalSize;
			totalSize = size.x * size.y;
			cols = res.x * 3;
			// Image Loop
			for (int i = 0; i < res.y; i++){
				for (int j = 0; j < cols; j += 3){
					img_pos = glm::ivec2(i * cols, j);
					// Kernel Loop
					for (int ii = 0; ii < k_size.x; ii++) {
						for (int jj = 0; jj < k_size.y; jj++) {
							rgb_offset = (jj - pivot.y) * 3;
							k_pos = glm::ivec2(img_pos.x + (ii - pivot.x) * cols, img_pos.y + rgb_offset);
							if (!isInImage(k_pos.x, k_pos.y))
								continue;
							rgb_array.push_back(glm::ivec3(
								imageData[k_pos.x + (k_pos.y + 0)],
								imageData[k_pos.x + (k_pos.y + 1)],
								imageData[k_pos.x + (k_pos.y + 2)]
							));
						}
					}

					// Sort colors
					sort(rgb_array);
					// Assign median color to output image
					output[i * cols + (j + 0)] = rgb_array[rgb_array.size() / 2].r;
					output[i * cols + (j + 1)] = rgb_array[rgb_array.size() / 2].g;
					output[i * cols + (j + 2)] = rgb_array[rgb_array.size() / 2].b;
					rgb_array.clear();
				}
			}
			createTexture2D(cpuTexture, output, res);
		}
		shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
		image = cpuTexture;
	}
	currentShader = DIMG_MEDIAN_BLUR;
	shader->use();
	// Send image to GPU
	shader->setInt("image", 0);
	shader->setInt("kernel", 1);
	shader->setInt("kWidth", size.x);
	shader->setInt("kHeight", size.y);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, kernel);
}

void DIMG::gaussianLaplace(GLuint image, bool hardwareAcceleration) {
	if (hardwareAcceleration) {
		if (currentShader != DIMG_LOG_GRAD || k_size != size || cc != c) {
			currentShader = DIMG_LOG_GRAD;
			setKernel(hardwareAcceleration);
			k_size = size;
			cc = c;
			shader = new Shader("assets/shaders/gaussianLaplace.vert", "assets/shaders/gaussianLaplace.frag");
		}
	}
	else {
		if (currentShader != DIMG_LOG_GRAD || k_size != size || cc != c) {
			currentShader = DIMG_LOG_GRAD;
			setKernel(hardwareAcceleration);
			k_size = size;
			cc = c;
			unsigned char* output = new unsigned char[res.x * res.y * 3];
			memset(output, 0, res.x * res.y * 3);
			glm::vec3  LoG_gx;
			glm::ivec2 pivot = glm::ivec2(k_size.x / 2, k_size.y / 2);
			glm::ivec2 img_pos, k_pos;
			int cols, rgb_offset, totalSize;
			totalSize = size.x * size.y;
			cols = res.x * 3;
			// Image Loop
			for (int i = 0; i < res.y; i++) {
				for (int j = 0; j < cols; j += 3) {
					LoG_gx = glm::vec3(0);
					img_pos = glm::ivec2(i * cols, j);
					// Kernel Loop
					for (int ii = 0; ii < k_size.x; ii++) {
						//printf("--------------------- KROW: %i\n", ii);
						for (int jj = 0; jj < k_size.y; jj++) {
							rgb_offset = (jj - pivot.y) * 3;
							k_pos = glm::ivec2(img_pos.x + (ii - pivot.x) * cols, img_pos.y + rgb_offset);
							if (!isInImage(k_pos.x, k_pos.y))
								continue;
							/*					printf("[img]rgb(%i,%i,%i)\n",
													imageData[k_pos.x + (k_pos.y + 0)],
													imageData[k_pos.x + (k_pos.y + 1)],
													imageData[k_pos.x + (k_pos.y + 2)]);*/
						// X-axis Gradient
							// red
							LoG_gx.r += imageData[k_pos.x + (k_pos.y + 0)] * gxData[ii * 7 + jj];
							// green
							LoG_gx.g += imageData[k_pos.x + (k_pos.y + 1)] * gxData[ii * 7 + jj];
							// blue
							LoG_gx.b += imageData[k_pos.x + (k_pos.y + 2)] * gxData[ii * 7 + jj];
						}
						//printf("\n");
					}
					// Edge detection final per pixel color
					output[i * cols + (j + 0)] = (int)LoG_gx.r;
					output[i * cols + (j + 1)] = (int)LoG_gx.g;
					output[i * cols + (j + 2)] = (int)LoG_gx.b;
				}
			}
			createTexture2D(cpuTexture, output, res);
		}
		shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
		image = cpuTexture;
	}
	shader->use();
	// Send image to GPU
	shader->setInt("image", 0);
	shader->setInt("gx", 1);
	shader->setInt("kWidth", size.x);
	shader->setInt("kHeight", size.y);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, gx);
}

void DIMG::toonShading(GLuint image, GLuint median, GLuint sobel) {
	shader = new Shader("assets/shaders/toonshading.vert", "assets/shaders/toonshading.frag");
	shader->use();
	// Send image to GPU
	shader->setInt("image", 0);
	shader->setInt("median", 1);
	shader->setInt("sobel", 2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, median);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, sobel);
}

void DIMG::setKernel(bool hardwareAcceleration){
	// Initialize Kernel information

	switch (currentShader) {
	case DIMG_MEAN_BLUR: {
		kernelData = vector<int>(49, 0);
		for (int i = 0; i < size.x; i++) {
			for (int j = 0; j < size.y; j++) {
				// m[i][j] = m[i * 7 + j] 
				// i y j empiezan en 0
				kernelData[i * 7 + j] = 1;
			}
		}
		if (hardwareAcceleration) {
			// Textures for mean blur
			glGenTextures(1, &kernel);
			glBindTexture(GL_TEXTURE_1D, kernel);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage1D(GL_TEXTURE_1D, 0, GL_R32I, 49, 0, GL_RED_INTEGER, GL_INT, kernelData.data());
			glBindTexture(GL_TEXTURE_1D, 0);
		}
		break;
	}
	case DIMG_SOBEL_GRAD: {
		gxData = std::vector<float>(49, 0.0f);
		gyData = std::vector<float>(49, 0.0f);
		glm::ivec2 pivot;
		int ki, kj, kiikjj;
		// Starting from kernel's center
		// Gx_ij = i / (i*i + j*j)
		// Gy_ij = j / (i*i + j*j)
		// GradientValue = sqrt(gx*gx + gy*gy)
		pivot.x = size.x / 2;
		pivot.y = size.y / 2;
		for (int i = 0; i < size.x; i++) {
			for (int j = 0; j < size.y; j++) {

				ki = i - pivot.x;
				kj = j - pivot.y;

				if (glm::ivec2(i, j) == pivot) {
					gxData[i * 7 + j] = 0;
					gyData[i * 7 + j] = 0;
					std::cout << "0   ";
					continue;
				}
				// Gx_ij = i / (i*i + j*j)
				kiikjj = ki * ki + kj * kj;
				//std::cout << kj << "/" << kiikjj << " ";
				gxData[i * 7 + j] = (float)ki / (float)kiikjj;
				gyData[i * 7 + j] = (float)kj / (float)kiikjj;
			}
		}

		//cout << "Printing gx" << endl;
		//for (int i = 0; i < 7; i++) {
		//	for (int j = 0; j < 7; j++) {
		//		cout << gx[i * 7 + j] << " ";
		//	}
		//	cout << endl;
		//}

		//cout << "Printing gy" << endl;
		//for (int i = 0; i < 7; i++) {
		//	for (int j = 0; j < 7; j++) {
		//		cout << gy[i * 7 + j] << " ";
		//	}
		//	cout << endl;
		//}
		if (hardwareAcceleration) {
			// Texture for sobel's gradient x-axis
			glGenTextures(1, &this->gx);
			glBindTexture(GL_TEXTURE_1D, this->gx);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, gxData.data());
			glBindTexture(GL_TEXTURE_1D, 0);

			// Texture for sobel's gradient y-axis
			glGenTextures(1, &this->gy);
			glBindTexture(GL_TEXTURE_1D, this->gy);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, gyData.data());
			glBindTexture(GL_TEXTURE_1D, 0);
		}
		break;
	}
	case DIMG_ROBERTS_GRAD: {
		gxData = std::vector<float>(49, 0.0f);
		gyData = std::vector<float>(49, 0.0f);
		glm::ivec2 pivot;
		int val;
		// CASO IMPAR: Buscar el numero par mas cercano por debajo y trabajar con esa matriz
		// CASO PAR: Matriz con diagonal espejo con signo cambiado
		/* 
		 PAR:          IMPAR:
		  2  0  0  0   0  0  0
		  0  1  0  0   0  1  0
		  0  0 -1  0   0  0 -1
		  0  0  0 -2

		*/
		pivot.x = size.x / 2;
		pivot.y = size.y / 2;
		val = pivot.x;
		for (int i = 0, j = 0; i < size.x; i++, j++, val--) {
			if (!val)
				val--;
			gxData[i * 7 + j] = (float)val;
		}

		val = pivot.y;
		for (int i = 0, j = size.y-1; i < size.x; i++, j--, val--) {
			if (!val)
				val--;
			gyData[i * 7 + j] = (float) val;
		}


		//cout << "Printing gx" << endl;
		//for (int i = 0; i < size.x; i++) {
		//	for (int j = 0; j < size.y; j++) {
		//		cout << gx[i * 7 + j] << " ";
		//	}
		//	cout << endl;
		//}

		//cout << "Printing gy" << endl;
		//for (int i = 0; i < size.x; i++) {
		//	for (int j = 0; j < size.y; j++) {
		//		cout << gy[i * 7 + j] << " ";
		//	}
		//	cout << endl;
		//}
		if (hardwareAcceleration) {
			// Texture for sobel's gradient x-axis
			glGenTextures(1, &this->gx);
			glBindTexture(GL_TEXTURE_1D, this->gx);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, gxData.data());
			glBindTexture(GL_TEXTURE_1D, 0);

			// Texture for sobel's gradient y-axis
			glGenTextures(1, &this->gy);
			glBindTexture(GL_TEXTURE_1D, this->gy);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, gyData.data());
			glBindTexture(GL_TEXTURE_1D, 0);
		}
		break;
	}
	case DIMG_PREWITT_GRAD: {
		// m[i][j] = m[i * 7 + j] 
		// i y j empiezan en 0
		gxData = std::vector<float>(49, 0.0f);
		gyData = std::vector<float>(49, 0.0f);

		glm::ivec2 pivot;
		int ki, kj;
		// Starting from kernel's center
		// Gx_ij = i / (i*i + j*j)
		// Gy_ij = j / (i*i + j*j)
		// GradientValue = sqrt(gx*gx + gy*gy)
		pivot.x = size.x / 2;
		pivot.y = size.y / 2;

		printf("(%i,%i)\n", size.x, size.y);

		for (int i = 0; i < size.x; i++) {

			for (int j = 0; j < size.y; j++) {

				ki = i - pivot.x;
				kj = j - pivot.y;

				// Gx_ij = i / (i*i + j*j)
				//std::cout << kj << "/" << kiikjj << " ";
				gxData[i * 7 + j] = (float)ki;
				gyData[i * 7 + j] = (float)kj;

			}
		}

		//cout << "Printing gx" << endl;
		//for (int i = 0; i < 7; i++) {
		//	for (int j = 0; j < 7; j++) {
		//		cout << gx[i * 7 + j] << " ";
		//	}
		//	cout << endl;
		//}

		//cout << "Printing gy" << endl;
		//for (int i = 0; i < 7; i++) {
		//	for (int j = 0; j < 7; j++) {
		//		cout << gy[i * 7 + j] << " ";
		//	}
		//	cout << endl;
		//}
		if (hardwareAcceleration) {
			// Texture for prewitt's gradient x-axis
			glGenTextures(1, &this->gx);
			glBindTexture(GL_TEXTURE_1D, this->gx);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, gxData.data());
			glBindTexture(GL_TEXTURE_1D, 0);

			// Texture for prewitt's gradient y-axis
			glGenTextures(1, &this->gy);
			glBindTexture(GL_TEXTURE_1D, this->gy);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, gyData.data());
			glBindTexture(GL_TEXTURE_1D, 0);
		}
		break;
	}
	case DIMG_LOG_GRAD: {
		gxData = std::vector<float>(49, 0.0f);
		glm::ivec2 pivot;
		int ki, kj, kii, kjj;
		pivot.x = size.x / 2;
		pivot.y = size.y / 2;
		float cc2 = c * c * 2;
		float cc4 = c*c*c*c;

		//cout << "x: " << x << endl << "cc2: "<<cc2;
		for (int i = 0; i < size.x; i++) {
			for (int j = 0; j < size.y; j++) {
				ki = i - pivot.x;
				kj = j - pivot.y;
				kii = ki * ki;
				kjj = kj * kj;
				gxData[i * 7 + j] = (kii + kjj - cc2) / cc4 * exp(-(kii + kjj) / cc2);
				//LoG[i * 7 + j] = x * (1-(kii+kjj)/cc2)* exp(-(kii + kjj) / cc2);
			}
		}

		//cout << "Printing LoG" << endl;
		//for (int i = 0; i < size.x; i++) {
		//	for (int j = 0; j < size.y; j++) {
		//		cout << LoG[i * 7 + j] << " ";
		//	}
		//	cout << endl;
		//}
		if (hardwareAcceleration) {
			// Texture for sobel's gradient x-axis
			glGenTextures(1, &this->gx);
			glBindTexture(GL_TEXTURE_1D, this->gx);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, gxData.data());
			glBindTexture(GL_TEXTURE_1D, 0);
		}
		break;
	}
	}
}

std::string DIMG::loadPath(){
	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "JPG Files(.jpg)\0*.jpg\0PNG Files(.png)\0*.png\0JPEG Files(.jpeg)\0*.jpeg;";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	ofn.lpstrDefExt = "";
	ofn.lpstrTitle = "Select an image";
	std::string fileNameStr;
	if (GetOpenFileName(&ofn)) {
		fileNameStr = fileName;
	}
	return fileNameStr;
}

std::string DIMG::savePath(){
	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "JPG Files(.jpg)\0*.jpg\0PNG Files(.png)\0*.png\0JPEG Files(.jpeg)\0*.jpeg;";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	ofn.lpstrDefExt = "";
	ofn.lpstrTitle = "Save image as...";
	std::string fileNameStr;
	if (GetSaveFileNameA(&ofn)) {
		fileNameStr = fileName;
	}
	return fileNameStr;
}

void DIMG::computeHistogram(GLuint image) {

	cout << "Creando histograma" << endl;

	int totalBytes = res.x * res.y * 3;
	int maxHeight = 0;

	//structure to check top for all colors
	int tops[256];
	memset(tops, 0, sizeof(tops));

	unsigned char redColor;

	for (int i = 0; i < totalBytes; i += 3) {

		//get red from image
		redColor = imageData[i];

		tops[redColor]++;

		if (tops[redColor] > maxHeight) maxHeight = tops[redColor];

		//cout << tops[redColor] << endl;
	}

	/*

	for (int i = 0; i < 256; i++) {
		cout << (int)tops[i] << endl;;
	}

	*/

	cout << "el tamanio maximo es: " << maxHeight << endl;

	int histogramWidth = 256;
	int histogramHeight = 150;
	int numChannels = 3;

	int totalBytesHist = histogramHeight * histogramWidth * numChannels;

	unsigned char* redHistogram = new unsigned char[totalBytesHist];

	for (int i = 0; i < totalBytesHist; i++) {
		redHistogram[i] = 255;
	}

	int tops2[256];
	memset(tops2, 0, sizeof(tops2));

	for (int i = 0; i < totalBytes; i += 3) {

		//get red from image
		redColor = imageData[i];

		float ratio = (float)tops2[redColor] / (float)maxHeight;

		int ii = (int)(ratio * histogramHeight);

		//assign red to image
		redHistogram[ii * histogramWidth * 3 + redColor * 3] = 0;
		redHistogram[ii * histogramWidth * 3 + redColor * 3 + 1] = 0;
		redHistogram[ii * histogramWidth * 3 + redColor * 3 + 2] = 0;

		tops2[redColor]++;

		//cout << tops[redColor] << endl;
	}

	unsigned char* redHistogramFlipped = new unsigned char[totalBytesHist];

	for (int i = 0; i < histogramHeight; i++) {

		for (int j = 0; j < histogramWidth * 3; j++) {

			unsigned char value;
			value = redHistogram[(histogramHeight - i - 1) * histogramWidth * 3 + j];
			redHistogramFlipped[i * histogramWidth * 3 + j] = value;
		}
	}


	cout << "saving image" << endl;
	glGenTextures(1, &histogram[0]);
	// Binds the texture
	glBindTexture(GL_TEXTURE_2D, histogram[0]);
	// Creates the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, histogramWidth, histogramHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, redHistogramFlipped);
	// Creates the texture mipmaps
	glGenerateMipmap(GL_TEXTURE_2D);

	// Set the filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	cout << "histogram saved" << endl;
}

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
	flag = 0;
	c = 1.0f;
	cc = c;
}

DIMG::~DIMG(){
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
	// We dont need the data texture anymore because is loaded on the GPU
	stbi_image_free(data);

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

void DIMG::negative(GLuint image){
	shader = new Shader("assets/shaders/negative.vert", "assets/shaders/negative.frag");
	currentShader = DIMG_NEGATIVE;
	shader->use();
	// Send image to GPU
	shader->setInt("image", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image);
}

void DIMG::grayscale(GLuint image) {
	shader = new Shader("assets/shaders/grayscale.vert", "assets/shaders/grayscale.frag");
	currentShader = DIMG_GRAYSCALE;
	shader->use();
	// Send image to GPU
	shader->setInt("image", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image);
}

void DIMG::blackandwhite(GLuint image) {
	shader = new Shader("assets/shaders/blackandwhite.vert", "assets/shaders/blackandwhite.frag");
	currentShader = DIMG_BLACKANDWHITE;
	shader->use();
	// Send image to GPU
	shader->setInt("image", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image);
}

void DIMG::sobel(GLuint image){
	if(!flag || (flag == 1 && currentShader != DIMG_SOBEL_GRAD) || k_size != size){
		currentShader = DIMG_SOBEL_GRAD;
		setKernel();
		flag = 1;
		k_size = size;
	}
	shader = new Shader("assets/shaders/sobel.vert", "assets/shaders/sobel.frag");
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

void DIMG::roberts(GLuint image) {
	if (!flag || (flag == 1 && currentShader != DIMG_ROBERTS_GRAD) || k_size != size) {
		currentShader = DIMG_ROBERTS_GRAD;
		setKernel();
		flag = 1;
		k_size = size;
	}
	shader = new Shader("assets/shaders/roberts.vert", "assets/shaders/roberts.frag");
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

void DIMG::prewitt(GLuint image) {
	if (!flag || (flag == 1 && currentShader != DIMG_PREWITT_GRAD) || k_size != size) {
		currentShader = DIMG_PREWITT_GRAD;
		setKernel();
		flag = 1;
		k_size = size;
	}
	shader = new Shader("assets/shaders/prewitt.vert", "assets/shaders/prewitt.frag");
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

void DIMG::mean(GLuint image) {
	if (!flag || (flag == 1 && currentShader != DIMG_MEAN_BLUR) || k_size != size) {
		currentShader = DIMG_MEAN_BLUR;
		setKernel();
		flag = 1;
		k_size = size;
	}
	shader = new Shader("assets/shaders/mean.vert", "assets/shaders/mean.frag");
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

void DIMG::median(GLuint image) {
	shader = new Shader("assets/shaders/median.vert", "assets/shaders/median.frag");
	currentShader = DIMG_MEDIAN_BLUR;
	shader->use();
	// Send image to GPU
	shader->setInt("image", 0);
	shader->setInt("kWidth", size.x);
	shader->setInt("kHeight", size.y);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image);
}

void DIMG::gaussianLaplace(GLuint image) {
	if (!flag || (flag == 1 && currentShader != DIMG_LOG_GRAD) || k_size != size || cc != c) {
		currentShader = DIMG_LOG_GRAD;
		setKernel();
		flag = 1;
		k_size = size;
		cc = c;
	}
	shader = new Shader("assets/shaders/gaussianLaplace.vert", "assets/shaders/gaussianLaplace.frag");
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

void DIMG::setKernel(){
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
		// Textures for mean blur
		glGenTextures(1, &kernel);
		glBindTexture(GL_TEXTURE_1D, kernel);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_R32I, 49, 0, GL_RED_INTEGER, GL_INT, kernelData.data());
		glBindTexture(GL_TEXTURE_1D, 0);
		break;
	}
	case DIMG_SOBEL_GRAD: {
		std::vector<float> gx = std::vector<float>(49, 0.0f);
		std::vector<float> gy = std::vector<float>(49, 0.0f);
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
					gy[i * 7 + j] = 0;
					gx[i * 7 + j] = 0;
					std::cout << "0   ";
					continue;
				}
				// Gx_ij = i / (i*i + j*j)
				kiikjj = ki * ki + kj * kj;
				//std::cout << kj << "/" << kiikjj << " ";
				gy[i * 7 + j] = (float)ki / (float)kiikjj;
				gx[i * 7 + j] = (float)kj / (float)kiikjj;
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

		// Texture for sobel's gradient x-axis
		glGenTextures(1, &this->gx);
		glBindTexture(GL_TEXTURE_1D, this->gx);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, gx.data());
		glBindTexture(GL_TEXTURE_1D, 0);

		// Texture for sobel's gradient y-axis
		glGenTextures(1, &this->gy);
		glBindTexture(GL_TEXTURE_1D, this->gy);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, gy.data());
		glBindTexture(GL_TEXTURE_1D, 0);
		break;
	}
	case DIMG_ROBERTS_GRAD: {
		std::vector<float> gx = std::vector<float>(49, 0.0f);
		std::vector<float> gy = std::vector<float>(49, 0.0f);
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
			gx[i * 7 + j] = val;
		}

		val = pivot.y;
		for (int i = 0, j = size.y-1; i < size.x; i++, j--, val--) {
			if (!val)
				val--;
			gy[i * 7 + j] = val;
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

		// Texture for sobel's gradient x-axis
		glGenTextures(1, &this->gx);
		glBindTexture(GL_TEXTURE_1D, this->gx);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, gx.data());
		glBindTexture(GL_TEXTURE_1D, 0);

		// Texture for sobel's gradient y-axis
		glGenTextures(1, &this->gy);
		glBindTexture(GL_TEXTURE_1D, this->gy);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, gy.data());
		glBindTexture(GL_TEXTURE_1D, 0);
		break;
	}
	case DIMG_PREWITT_GRAD: {
		// m[i][j] = m[i * 7 + j] 
		// i y j empiezan en 0
		std::vector<float> gx = std::vector<float>(49, 0.0f);
		std::vector<float> gy = std::vector<float>(49, 0.0f);

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
				gx[i * 7 + j] = (float)ki;
				gy[i * 7 + j] = (float)kj;

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

		// Texture for prewitt's gradient x-axis
		glGenTextures(1, &this->gx);
		glBindTexture(GL_TEXTURE_1D, this->gx);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, gx.data());
		glBindTexture(GL_TEXTURE_1D, 0);

		// Texture for prewitt's gradient y-axis
		glGenTextures(1, &this->gy);
		glBindTexture(GL_TEXTURE_1D, this->gy);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, gy.data());
		glBindTexture(GL_TEXTURE_1D, 0);
		break;
	}
	case DIMG_LOG_GRAD: {
		std::vector<float> LoG = std::vector<float>(49, 0.0f);
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
				LoG[i * 7 + j] = (kii + kjj - cc2) / cc4 * exp(-(kii + kjj) / cc2);
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

		// Texture for sobel's gradient x-axis
		glGenTextures(1, &this->gx);
		glBindTexture(GL_TEXTURE_1D, this->gx);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, LoG.data());
		glBindTexture(GL_TEXTURE_1D, 0);
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

void DIMG::createHistogram(GLuint image) {

	cout << "Creando histograma" << endl;

	//suppose image size is 800x600 just for practice purposes

	int imageWidth = 800;
	int imageHeight = 600;

	GLubyte* data = new GLubyte[imageWidth * imageHeight * 3];
	glBindTexture(GL_TEXTURE_2D, image);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	//int numChannels = 3;
	//string imgPath = saveImage(image);
	//unsigned char* data = stbi_load(imgPath.c_str(), &imageWidth, &imageHeight, &numChannels, 0);


	if (data) {

		cout << "imagen a histogramear carga" << endl;

		unsigned char* redHistogram = new unsigned char[imageWidth * imageHeight * 255]; //for channel

		//memset(redHistogram, NULL, sizeof(redHistogram));
		for (int i = 0; i < imageWidth * imageHeight * 255; i++) {
			redHistogram[i] = 255;
		}

		int totalBytes = imageWidth * imageHeight * 3;

		//structure to check top for all colors
		int tops[256];
		memset(tops, 0, sizeof(tops));

		int histogramWidth = 255;
		int histogramHeight = imageWidth * imageHeight;

		int maxHeight = 0;


		for (int i = 0; i < totalBytes; i += 3) {

			//get red from image
			unsigned char redColor = data[i];

			//cout << "tops de red color: "<<tops[redColor] << endl;

			//assign red to image
			redHistogram[tops[redColor] * 255 + redColor] = 0;

			tops[redColor]++;

			if (tops[redColor] > maxHeight) maxHeight = tops[redColor];

		}


		cout << "saving image" << endl;
		// flag is non-zero to flip data vertically
		//stbi_flip_vertically_on_write(true);
		std::string path = savePath();
		stbi_write_jpg(path.c_str(), histogramWidth, maxHeight, 1, redHistogram, 100);

		cout << "histogram saved" << endl;
	}
	// We dont need the data texture anymore because is loaded on the GPU
	stbi_image_free(data);
}

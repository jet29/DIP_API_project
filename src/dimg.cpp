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
	old_size = size;
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

	//bubble sort

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

GLuint DIMG::loadImage(string path) {

	if (path == "") {
		// Using Windows OS primitive to get retrieve paths
		std::string winPath = loadPath();
		path = winPath;
	}
	
	unsigned int id;
	// Creates the texture on GPU
	glGenTextures(1, &id);
	// Flips the texture when loads it because in opengl the texture coordinates are flipped
	stbi_set_flip_vertically_on_load(true);
	// Loads the texture file data
	imageData = stbi_load(path.c_str(), &res.x, &res.y, &bpp, 0);
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
	shader = new Shader("assets/shaders/negative.vert", "assets/shaders/negative.frag");
	currentShader = DIMG_NEGATIVE;
	shader->use();
	shader->setInt("image", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image);
}

void DIMG::grayscale(GLuint image, bool hardwareAcceleration) {
	shader = new Shader("assets/shaders/grayscale.vert", "assets/shaders/grayscale.frag");
	currentShader = DIMG_GRAYSCALE;
	shader->use();
	shader->setInt("image", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image);
}

void DIMG::blackandwhite(GLuint image, bool hardwareAcceleration) {
	shader = new Shader("assets/shaders/blackandwhite.vert", "assets/shaders/blackandwhite.frag");
	currentShader = DIMG_BLACKANDWHITE;
	shader->use();
	shader->setInt("image", 0);
	shader->setFloat("f_threshold", f_threshold);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image);
}

void DIMG::sobel(GLuint image, bool hardwareAcceleration){

	if (currentShader != DIMG_SOBEL_GRAD || old_size != size) {
		currentShader = DIMG_SOBEL_GRAD;
		setKernel(hardwareAcceleration);
		old_size = size;
		shader = new Shader("assets/shaders/sobel.vert", "assets/shaders/sobel.frag");
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

	if (currentShader != DIMG_ROBERTS_GRAD || old_size != size) {
		currentShader = DIMG_ROBERTS_GRAD;
		setKernel(hardwareAcceleration);
		old_size = size;
		shader = new Shader("assets/shaders/roberts.vert", "assets/shaders/roberts.frag");
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
	
	if (currentShader != DIMG_PREWITT_GRAD || old_size != size) {
		currentShader = DIMG_PREWITT_GRAD;
		setKernel(hardwareAcceleration);
		old_size = size;
		shader = new Shader("assets/shaders/prewitt.vert", "assets/shaders/prewitt.frag");
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

	if (currentShader != DIMG_MEAN_BLUR || old_size != size) {
		currentShader = DIMG_MEAN_BLUR;
		setKernel(hardwareAcceleration);
		old_size = size;
		shader = new Shader("assets/shaders/mean.vert", "assets/shaders/mean.frag");
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

	if (currentShader != DIMG_MEDIAN_BLUR || old_size != size) {

		old_size = size;
		shader = new Shader("assets/shaders/median.vert", "assets/shaders/median.frag");
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
		if (currentShader != DIMG_LOG_GRAD || old_size != size || cc != c) {
			currentShader = DIMG_LOG_GRAD;
			setKernel(hardwareAcceleration);
			old_size = size;
			cc = c;
			shader = new Shader("assets/shaders/gaussianLaplace.vert", "assets/shaders/gaussianLaplace.frag");
		}
	}
	else {
		
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
					//std::cout << "0   ";
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

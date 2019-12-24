#include "dimg.h"

DIMG::DIMG(int windowWidth, int windowHeight)
{
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
	VAO = VBO = 0;
	//init();
}

DIMG::DIMG()
{
	// If not speficied API assumes standard 800x600 viewport size
	windowWidth  = 800;
	windowHeight = 600;
	VAO = VBO = 0;
	//init();
}

void DIMG::setArrayAndBufferObjects(GLuint VAO, GLuint VBO) {
	this->VAO = VAO;
	this->VBO = VBO;
	//buildScreenQuad(VAO, VBO);
}



DIMG::~DIMG()
{
}

void DIMG::init(){
	initWindow();
	initGlad();
	// Initialize OpenGL context
	// Enables the z-buffer test
	glEnable(GL_DEPTH_TEST);
	// Sets the ViewPort
	glViewport(0, 0, windowWidth, windowHeight);
	// Sets the clear color
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}

/***********************************
 ********* PUBLIC FUNCTIONS ********
 ***********************************/

unsigned int DIMG::loadImage(const char *path)
{
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

unsigned int DIMG::negative(GLuint image) {
	Shader *shader = new Shader("src/dimg/shaders/negative/negative.vert", "src/dimg/shaders/negative/negative.frag");
	setFrameBuffer();

	/* RENDER NEGATIVE IMAGE TO TEXTURE */
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// Clears the color and depth buffers from the frame buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/** Draws code goes here **/
	// Use the shader
	shader->use();
	// Send image to GPU
	shader->setInt("image", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image);
	// Binds the vertex array to be drawn
	glBindVertexArray(VAO);
	// Renders the triangle gemotry
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	return dsTexture;

}

/***********************************
 ********* PRIVATE FUNCTIONS *******
 ***********************************/

bool DIMG::setFrameBuffer() {
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// The texture we're going to render to
	glGenTextures(1, &dsTexture);
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, dsTexture);

	// Give an empty image to OpenGL (Which is done with last "0")
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Depth buffer
	glGenRenderbuffers(1, &depthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

	// Frame buffer configuration
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, dsTexture, 0);

	// Set list of draw buffers
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);



	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::Framebuffer configuration went wrong" << std::endl;
		return false;
	}
	return true;
}

void DIMG::buildScreenQuad(GLuint &VAO, GLuint &VBO) {
	// Quad for debug purposes:
	float quadVertices[] = {
		// positions        // Color   		   // texture Coords
		-1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 0.5f, 0.5f, 0.75f, 1.0f, 0.0f,
	};
	// Setup plane VAO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	// Color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	// Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
}

bool DIMG::initWindow(){
	// Initialize glfw
	glfwInit();
	// Sets the Opegl context to Opengl 3.0
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	// Creates the window
	window = glfwCreateWindow(windowWidth, windowHeight, "Can't see this windows", NULL, NULL);

	// The window couldn't be created
	if (!window)
	{
		std::cout << "Failed to create the glfw window" << std::endl;
		glfwTerminate(); // Stops the glfw program
		return false;
	}

	// Creates the glfwContext, this has to be made before calling initGlad()
	glfwMakeContextCurrent(window);

	// Window resize callback
	//glfwSetFramebufferSizeCallback(window, resize);
	return true;
}

/**
 * Initialize the glad library
 * @returns{bool} true if everything goes ok
 * */
bool DIMG::initGlad(){
	// Initialize glad
	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	// If something went wrong during the glad initialization
	if (!status)
	{
		std::cout << status << std::endl;
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	return true;
}

void DIMG::initGL(){
	// Enables the z-buffer test
	glEnable(GL_DEPTH_TEST);
	// Sets the ViewPort
	glViewport(0, 0, windowWidth, windowHeight);
	// Sets the clear color
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}

void DIMG::resize(GLFWwindow *window, int width, int height){
	windowWidth = width;
	windowHeight = height;
	// Sets the OpenGL viewport size and position
	glViewport(0, 0, windowWidth, windowHeight);
}




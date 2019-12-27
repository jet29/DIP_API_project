#include <glad/glad.h> // Glad has to be include before glfw
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <stb_image.h>
#include <Commdlg.h>
#include "Shader.h"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Window current width
unsigned int windowWidth = 800;
// Window current height
unsigned int windowHeight = 600;
// Window title
const char *windowTitle = "Basic Demo";
// Window pointer
GLFWwindow *window;

// Shader object
Shader *shader, *negative, *grayscale, *blackandwhite;
// Index (GPU) of the geometry buffer
unsigned int VBO;
// Index (GPU) vertex array object
unsigned int VAO;
// Index (GPU) of the texture
unsigned int textureID, imageID;
// Deferred shading textures, buffers
GLuint framebuffer, depthBuffer;
GLuint dsTexture;

bool setFrameBuffer();
std::string loadPath();

/**
 * Handles the window resize
 * @param{GLFWwindow} window pointer
 * @param{int} new width of the window
 * @param{int} new height of the window
 * */
void resize(GLFWwindow *window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    // Sets the OpenGL viewport size and position
    glViewport(0, 0, windowWidth, windowHeight);
}
/**
 * Initialize the glfw library
 * @returns{bool} true if everything goes ok
 * */
bool initWindow()
{
    // Initialize glfw
    glfwInit();
    // Sets the Opegl context to Opengl 3.0
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Creates the window
    window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);

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
    glfwSetFramebufferSizeCallback(window, resize);
    return true;
}
/**
 * Initialize the glad library
 * @returns{bool} true if everything goes ok
 * */
bool initGlad()
{
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
/**
 * Initialize the opengl context
 * */
void initGL()
{
    // Enables the z-buffer test
    glEnable(GL_DEPTH_TEST);
    // Sets the ViewPort
    glViewport(0, 0, windowWidth, windowHeight);
    // Sets the clear color
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}
/**
 * Builds all the geometry buffers and
 * loads them up into the GPU
 * (Builds a simple triangle)
 * */

void buildGeometry()
{
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
/**
 * Loads a texture into the GPU
 * @param{const char} path of the texture file
 * @returns{unsigned int} GPU texture index
 * */

unsigned int loadTexture(const char *path)
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

void saveImage() {
	GLubyte* pixels = new GLubyte[800 * 600 * 3];
	glBindTexture(GL_TEXTURE_2D, dsTexture);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	// You have to use 3 comp for complete jpg file. If not, the image will be grayscale or nothing.
	stbi_flip_vertically_on_write(true); // flag is non-zero to flip data vertically
	stbi_write_jpg("zelda.jpg", 800, 600, 3, pixels, 100);
}


/**
 * Initialize everything
 * @returns{bool} true if everything goes ok
 * */
bool init()
{
    // Initialize the window, and the glad components
    if (!initWindow() || !initGlad() || !setFrameBuffer())
        return false;

    // Initialize the opengl context
    initGL();

    // Loads the shader
    shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
	negative = new Shader("assets/shaders/negative.vert", "assets/shaders/negative.frag");
	grayscale = new Shader("assets/shaders/grayscale.vert", "assets/shaders/grayscale.frag");
	blackandwhite = new Shader("assets/shaders/blackandwhite.vert", "assets/shaders/blackandwhite.frag");

    // Loads all the geometry into the GPU
    buildGeometry();
    // Loads the texture into the GPU
    textureID = loadTexture("assets/textures/bricks2.jpg");
	imageID = loadTexture("assets/images/zelda.jpg");

    return true;
}
/**
 * Process the keyboard input
 * There are ways of implementing this function through callbacks provide by
 * the GLFW API, check the GLFW documentation to find more
 * @param{GLFWwindow} window pointer
 * */

void processKeyboardInput(GLFWwindow *window)
{
    // Checks if the escape key is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        // Tells glfw to close the window as soon as possible
        glfwSetWindowShouldClose(window, true);

    // Checks if the r key is pressed
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        // Reloads the shader
        delete shader;
		delete negative;
		delete grayscale;
		delete blackandwhite;

		shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
		negative = new Shader("assets/shaders/negative.vert", "assets/shaders/negative.frag");
		grayscale = new Shader("assets/shaders/grayscale.vert", "assets/shaders/grayscale.frag");
		blackandwhite = new Shader("assets/shaders/blackandwhite.vert", "assets/shaders/blackandwhite.frag");
    }

	// Load a new image
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		imageID = loadTexture(loadPath().c_str());
	}

	// Save current image
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		saveImage();
	}
}

std::string loadPath()
{
	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "JPG Files(.jpg)\0*.jpg\0PNG Files(.png)\0*.png\0JPEG Files(.jpeg)\0*.jpeg;";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "";
	ofn.lpstrTitle = "Select an image";
	std::string fileNameStr;
	if (GetOpenFileName(&ofn)) {
		fileNameStr = fileName;
	}
	std::cout << fileNameStr << std::endl;
	return fileNameStr;
}

void getNegative() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// Clears the color and depth buffers from the frame buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/** Draws code goes here **/
	// Use the shader
	negative->use();
	// Send image to GPU
	negative->setInt("image", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, imageID);
	// Binds the vertex array to be drawn
	glBindVertexArray(VAO);
	// Renders the triangle gemotry
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void getGrayscale() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// Clears the color and depth buffers from the frame buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/** Draws code goes here **/
	// Use the shader
	grayscale->use();
	// Send image to GPU
	grayscale->setInt("image", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, imageID);
	// Binds the vertex array to be drawn
	glBindVertexArray(VAO);
	// Renders the triangle gemotry
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void getBlackAndWhite() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// Clears the color and depth buffers from the frame buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/** Draws code goes here **/
	// Use the shader
	blackandwhite->use();
	// Send image to GPU
	blackandwhite->setInt("image", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, imageID);
	// Binds the vertex array to be drawn
	glBindVertexArray(VAO);
	// Renders the triangle gemotry
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}



/**
 * Render Function
 * */
void render()
{
	// Compute image's negative
	//getNegative();
	// Compute image's grayscale
	//getGrayscale();
	// Compute image's black and white
	getBlackAndWhite();
	///* RENDER NEGATIVE IMAGE TO TEXTURE */

	/* RENDER IMAGE */
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Use the shader
	shader->use();
	shader->setInt("image", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, dsTexture);
	//Draw QUAD for debug purposes
	//Binds the vertex array to be drawn
	glBindVertexArray(VAO);
	// Renders the triangle geometry
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	// Swap the buffer
	glfwSwapBuffers(window);
}
/**
 * App main loop
 * */
void update()
{
    // Loop until something tells the window, that it has to be closed
    while (!glfwWindowShouldClose(window))
    {
        // Checks for keyboard inputs
        processKeyboardInput(window);

        // Renders everything
        render();

        // Check and call events
        glfwPollEvents();
    }
}

bool setFrameBuffer() {
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
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

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

/**
 * App starting point
 * @param{int} number of arguments
 * @param{char const *[]} running arguments
 * @returns{int} app exit code
 * */
int main(int argc, char const *argv[])
{
    // Initialize all the app components
    if (!init())
    {
        // Something went wrong
        std::cin.ignore();
        return -1;
    }

    std::cout << "=====================================================" << std::endl
              << "        Press Escape to close the program            " << std::endl
              << "=====================================================" << std::endl;

    // Starts the app main loop
    update();
    // Deletes the texture from the gpu
    glDeleteTextures(1, &textureID);
    // Deletes the vertex array from the GPU
    glDeleteVertexArrays(1, &VAO);
    // Deletes the vertex object from the GPU
    glDeleteBuffers(1, &VBO);
    // Destroy the shader
    delete shader;

    // Stops the glfw program
    glfwTerminate();

    return 0;
}

#include <glad/glad.h> // Glad has to be include before glfw
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include "dimg.h"
#include "Shader.h"
#include "UserInterface.h"

typedef enum {REGULAR,NEGATIVE,GRAYSCALE,BLACKANDWHITE,SOBEL,ROBERTS,PREWITT,MEAN,MEDIAN,GAUSSIAN,TOON_SHADING} techniques;

DIMG api;
UI *ui;
// Window current width
unsigned int windowWidth = 800;
// Window current height
unsigned int windowHeight = 600;
// Window title
const char *windowTitle = "API using interface";
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
// Deferred shading textures, buffers
GLuint framebuffer, depthBuffer;
GLuint dsTexture1, dsTexture2, dsTexture3;

/**
 * Handles the window resize
 * @param{GLFWwindow} window pointer
 * @param{int} new width of the window
 * @param{int} new height of the window
 * */
void resize(GLFWwindow *window, int width, int height);

/**
 * Initialize the glfw library
 * @returns{bool} true if everything goes ok
 * */
bool initWindow();

/**
 * Initialize the glad library
 * @returns{bool} true if everything goes ok
 * */
bool initGlad();

/**
 * Initialize the opengl context
 * */
void initGL();

/**
 * Builds all the geometry buffers and
 * loads them up into the GPU
 * (Builds a simple triangle)
 * */
void buildGeometry();

/**
 * Initialize everything
 * @returns{bool} true if everything goes ok
 * */
bool init();

/**
 * Process the keyboard input
 * There are ways of implementing this function through callbacks provide by
 * the GLFW API, check the GLFW documentation to find more
 * @param{GLFWwindow} window pointer
 * */
void processKeyboardInput(GLFWwindow *window);

/**
 * Render Function using Deferred shading
 * it changes default framebuffer for a custom one
 * */
void renderToTexture();

/**
 * Default OpenGL Render
 * */
void forwardRendering();

/**
 * Render Function
 * */
void render();

/**
 * App main loop
 * */
void update();

/**
 * Set OpenGL configuration for a
 * custom Frame Buffer
 * */
bool setFrameBuffer(GLuint& texture);

/**
 * App starting point
 * @param{int} number of arguments
 * @param{char const *[]} running arguments
 * @returns{int} app exit code
 * */
int main(int argc, char const *argv[]);

#include "main.h"

void resize(GLFWwindow *window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    // Sets the OpenGL viewport size and position
    glViewport(0, 0, windowWidth, windowHeight);
}

void onKeyPressCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_R:
			// Reloads the shader
			delete shader;
			shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
			api.reloadShader();
			break;
		case GLFW_KEY_L:
			glDeleteTextures(1, &imageID);
			api.loadImage();
			break;
		case GLFW_KEY_T:
			std::cout << "Tecnica: " << UI::listbox_item_current << std::endl;
			//ImGui::ShowDemoWindow();
			break;
		}
	}
}

bool initWindow(){
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
	// Window keys callback
	glfwSetKeyCallback(window, onKeyPressCallback);
    return true;
}

bool initGlad(){
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

void initGL()
{
    // Enables the z-buffer test
    glEnable(GL_DEPTH_TEST);
    // Sets the ViewPort
    glViewport(0, 0, windowWidth, windowHeight);
    // Sets the clear color
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}

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

bool init()
{
    // Initialize the window, and the glad components
    if (!initWindow() || !initGlad() || !setFrameBuffer() || !ui->initImGui(window))
        return false;

    // Initialize the opengl context
    initGL();

    // Loads the shader
    shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    
	// Loads all the geometry into the GPU
    buildGeometry();

    // Loads the texture into the GPU
	imageID = api.loadImage("assets/images/zelda.jpg");
	ui->setHistogram(imageID);

    return true;
}

void processKeyboardInput(GLFWwindow *window)
{
    // Checks if the escape key is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        // Tells glfw to close the window as soon as possible
        glfwSetWindowShouldClose(window, true);

	// Save current image
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		api.saveImage(dsTexture);
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		printf("kernel: (%i,%i)\n", UI::kernelheight, UI::kernelwidth);
	}
}

void renderToTexture(){
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// Clears the color and depth buffers from the frame buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/** Draws code goes here **/
	// Use the shader
	switch (UI::listbox_item_current) {
	case REGULAR: 
		api.color(imageID);
		break;
	case NEGATIVE: 
		api.negative(imageID);
		break;
	case GRAYSCALE: 
		api.grayscale(imageID);
		break;
	case BLACKANDWHITE:
		api.blackandwhite(imageID);
		break;
	case SOBEL:
		api.sobel(imageID);
		break;
	case ROBERTS:
		api.roberts(imageID);
		break;
	case PREWITT:
		api.prewitt(imageID);
		break;
	case MEAN:
		api.mean(imageID);
		break;
	case MEDIAN:
		api.median(imageID);
		break;
	case GAUSSIAN:
		api.gaussianLaplace(imageID);
		break;
	}
	// Binds the vertex array to be drawn
	glBindVertexArray(VAO);
	// Render triangle's geometry
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void forwardRendering() {
	// Forward rendering
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Use the shader
	shader->use();
	shader->setInt("image", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, dsTexture);
	//Binds the vertex array to be drawn
	glBindVertexArray(VAO);
	// Renders the triangle geometry
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void render()
{
	// Deferred shading
	renderToTexture();

	// Forward rendering
	forwardRendering();

	// Draw ImGui
	ui->ImGuiDraw();

	// Swap buffers
	glfwSwapBuffers(window);
}

void updateFromInterface() {
	if (api.getKernelSize() != glm::ivec2(UI::kernelheight,UI::kernelwidth))
		api.setKernelSize(UI::kernelheight, UI::kernelwidth);
}

void update()
{
    // Loop until something tells the window, that it has to be closed
    while (!glfwWindowShouldClose(window))
    {
        // Checks for keyboard inputs
        processKeyboardInput(window);

        // Render everything
        render();

		// Check and call events
        glfwPollEvents();

		// Update values
		updateFromInterface();
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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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

int main(int argc, char const *argv[])
{
	ui = new UI();
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
    glDeleteTextures(1, &imageID);
    // Deletes the vertex array from the GPU
    glDeleteVertexArrays(1, &VAO);
    // Deletes the vertex object from the GPU
    glDeleteBuffers(1, &VBO);
    // Destroy the shader
    delete shader;

    // Stops the glfw program
    glfwTerminate();

	// Terminate IMGUI
	ui->ImGuiTerminate();

    return 0;
}

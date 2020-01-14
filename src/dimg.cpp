#include "dimg.h"
#include "stb_image.cpp"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

dimg::dimg() {
}

void dimg::buildGeometry() {
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

bool dimg::initWindow() {
    // Initialize glfw
    glfwInit();
    // Sets the Opegl context to Opengl 3.0
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Creates the window
    window = glfwCreateWindow(windowWidth, windowHeight, "DIMG internal window", NULL, NULL);

    // The window couldn't be created
    if (!window)
    {
        std::cout << "Failed to create the glfw window" << std::endl;
        glfwTerminate(); // Stops the glfw program
        return false;
    }

    // Creates the glfwContext, this has to be made before calling initGlad()
    glfwMakeContextCurrent(window);

    return true;
}

bool dimg::initGlad() {
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

void dimg::initGL() {
    // Enables the z-buffer test
    glEnable(GL_DEPTH_TEST);
    // Sets the ViewPort
    glViewport(0, 0, windowWidth, windowHeight);
    // Sets the clear color
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}

bool dimg::init() {
	// Initialize window and glad components
	if (!initWindow() || !initGlad())
		return false;
	// Initialize OpenGL context
	initGL();
	// Initialize geometry for GPU
	buildGeometry();

    return true;
}

bool dimg::setFrameBuffer(int width, int height) {
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // The texture we're going to render to
    glGenTextures(1, &imageID);
    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, imageID);

    // Give an empty image to OpenGL (Which is done with last "0")
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Depth buffer
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    // Frame buffer configuration
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, imageID, 0);

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

bool dimg::createTexture2D(IMGDATA img, unsigned int &id) {
    if (img.data)
    {
        // Gets the texture channel format
        GLenum format;
        switch (img.numOfChannels)
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
        // Create ID on GPU
        glGenTextures(1, &id);
        // Binds the texture
        glBindTexture(GL_TEXTURE_2D, id);
        // Creates the texture
        glTexImage2D(GL_TEXTURE_2D, 0, format, img.width, img.height, 0, format, GL_UNSIGNED_BYTE, img.data);
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
        std::cout << "ERROR:: Unable to load texture " << img.path << std::endl;
        glDeleteTextures(1, &id);
        return false;
    }
    return true;
}

Shader* setShader(DIMG_ENUM type) {
    Shader* shader = new Shader("src/shaders/basic.vert", "src/shaders/basic.frag");
    switch (type) {
    case DIMG_NEGATIVE:
        shader = new Shader("src/shaders/negative.vert", "src/shaders/negative.frag");
        break;
    case DIMG_GRAYSCALE:
        shader = new Shader("src/shaders/grayscale.vert", "src/shaders/grayscale.frag");
        break;
    case DIMG_BLACK_AND_WHITE:
        shader = new Shader("src/shaders/blackandwhite.vert", "src/shaders/blackandwhite.frag");
        break;
    }
    // Send image to GPU
    shader->setInt("image", 0);
    shader->use();
    return shader;
}

void dimg::deferredShading(unsigned int id) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // Clears the color and depth buffers from the frame buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);
    // Binds the vertex array to be drawn
    glBindVertexArray(VAO);
    // Render triangle's geometry
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

bool dimg::loadImage(const char* path,IMGDATA &img) {
    img.path = path;
    // Flips the texture when loads it because in opengl the texture coordinates are flipped
    stbi_set_flip_vertically_on_load(true);
    // Loads the texture file data
    img.data = stbi_load(path, &img.width, &img.height, &img.numOfChannels, 0);
    if (!img.data) {
        std::cout << "ERROR:: Unable to load image " << img.path << std::endl;
        return false;
    }
    return true;
}

bool dimg::saveImage(IMGDATA img) {
    // You have to use 3 comp for complete jpg file. If not, the image will be grayscale or nothing.
    stbi_flip_vertically_on_write(true); // flag is non-zero to flip data vertically
    if (!stbi_write_jpg(img.path, img.width, img.height, img.numOfChannels, img.data, 90)) {
        std::cout << "ERROR:: Unable to save image " << img.path << std::endl;
        return false;
    }
    return true;
}

void dimg::initImage(IMGDATA &dest, unsigned char* data, int width, int height, int numOfChannels, const char* path){
    dest.data = new unsigned char[height * width * 3];
    dest.height = height;
    dest.width = width;
    dest.numOfChannels = numOfChannels;
    dest.path = path;
}

bool dimg::dimg_negative(const char* target, const char* dest, bool hwAcc) {
    // Load image information
    IMGDATA input_img;
    if (!loadImage(target, input_img))
        return false;
    // Output image init
    IMGDATA output_img;
    initImage(output_img, input_img.data, input_img.width, input_img.height, input_img.numOfChannels, dest);
    // Index (GPU) of dest texture
    unsigned int texID;
    // Create framebuffer
    setFrameBuffer(output_img.width, output_img.height);
    // Set viewport size to render in original resolution
    glViewport(0, 0, output_img.width, output_img.height);
    // GPU Implementation
    if (hwAcc) {
        // Create GPU Texture
        if (!createTexture2D(input_img, texID))
            return false;
        // Set shader configuration
        shader = setShader(DIMG_NEGATIVE);
        // Deferred shading
        deferredShading(texID);
        // Get image data from GPU
        glBindTexture(GL_TEXTURE_2D, imageID);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, output_img.data);
    }
    // CPU Implementation
    else {
        for (int i = 0; i < output_img.height * output_img.width * 3; i++)
            output_img.data[i] = 255 - input_img.data[i];
    }
    // Save image 
    if (!saveImage(output_img))
        return false;
    // Free space
    terminate_filter(texID, imageID, input_img, output_img);

    return true;
}

bool dimg::dimg_grayscale(const char* target, const char* dest, bool hwAcc) {
    // Load image information
    IMGDATA input_img;
    if (!loadImage(target, input_img))
        return false;
    // Output image init
    IMGDATA output_img;
    initImage(output_img, input_img.data, input_img.width, input_img.height, input_img.numOfChannels, dest);
    // Index (GPU) of dest texture
    unsigned int texID;
    // Create framebuffer
    setFrameBuffer(output_img.width, output_img.height);
    // Set viewport size to render in original resolution
    glViewport(0, 0, output_img.width, output_img.height);
   
    // GPU Implementation
    if (hwAcc) {
        // Create GPU Texture
        if (!createTexture2D(input_img, texID))
            return false;
        // Set shader configuration
        shader = setShader(DIMG_GRAYSCALE);
        // Deferred shading
        deferredShading(texID);
        // Get image data from GPU
        glBindTexture(GL_TEXTURE_2D, imageID);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, output_img.data);
    }
    // CPU Implementation
    else {
        int gs_value;
        for (int i = 0; i < input_img.height * input_img.width * 3; i++) {

            gs_value = (int)(input_img.data[i] * 0.2989) +
                       (int)(input_img.data[i + 1] * 0.5870) +
                       (int)(input_img.data[i + 2] * 0.1140);
            // Red
            output_img.data[i]     = gs_value;
            // Green
            output_img.data[i + 1] = gs_value;
            // Blue
            output_img.data[i + 2] = gs_value;
        }
    }
    // Save image 
    if (!saveImage(output_img))
        return false;
    // Free space
    terminate_filter(texID, imageID, input_img, output_img);

    return true;
}

bool dimg::dimg_black_and_white(const char* target, const char* dest, int threshold, bool hwAcc) {
    // Load image information
    IMGDATA input_img;
    if (!loadImage(target, input_img))
        return false;
    // Output image init
    IMGDATA output_img;
    initImage(output_img, input_img.data, input_img.width, input_img.height, input_img.numOfChannels, dest);
    // Index (GPU) of dest texture
    unsigned int texID;
    // Create framebuffer
    setFrameBuffer(output_img.width, output_img.height);
    // Set viewport size to render in original resolution
    glViewport(0, 0, output_img.width, output_img.height);
    
    // GPU Implementation
    if (hwAcc) {
        // Create GPU Texture
        if (!createTexture2D(input_img, texID))
            return false;
        // Set shader configuration
        shader = setShader(DIMG_BLACK_AND_WHITE);
        shader->setFloat("f_threshold", (float)threshold / (float)255);
        // Deferred shading
        deferredShading(texID);
        // Get image data from GPU
        glBindTexture(GL_TEXTURE_2D, imageID);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, output_img.data);
    }
    // CPU Implementation
    else {
        int gs_value, bnw;
        for (int i = 0; i < output_img.height * output_img.width * 3; i++) {
            gs_value = (int)(input_img.data[i] * 0.2989) +
                       (int)(input_img.data[i + 1] * 0.5870) +
                       (int)(input_img.data[i + 2] * 0.1140);
            bnw = gs_value > threshold ? 255 : 0;
            // Red
            output_img.data[i] = bnw;
            // Green
            output_img.data[i + 1] = bnw;
            // Blue
            output_img.data[i + 2] = bnw;
        }
    }
    // Save image 
    if (!saveImage(output_img))
        return false;
    // Free space
    terminate_filter(texID, imageID, input_img, output_img);

    return true;
}

void dimg::terminate_filter(unsigned int& tex1, unsigned int& tex2, IMGDATA &img1, IMGDATA &img2) {
    // Deletes texture from gpu
    glDeleteTextures(1, &tex1);
    // Deletes texture from gpu
    glDeleteTextures(1, &tex2);
    // Free image bytes on memory
    delete img1.data;
    // Free image bytes on memory
    delete img2.data;
}

void dimg::dimg_terminate() {

    // Deletes the vertex array from the GPU
    glDeleteVertexArrays(1, &VAO);
    // Deletes the vertex object from the GPU
    glDeleteBuffers(1, &VBO);
    // Destroy the shader
    delete shader;
    // Stops the glfw program
    glfwTerminate();
}
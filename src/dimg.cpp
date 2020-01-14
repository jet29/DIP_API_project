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

Shader* setShader(DIMGenum type) {
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
    case DIMG_MEAN_BLUR:
        shader = new Shader("src/shaders/mean.vert", "src/shaders/mean.frag");
        break;
    case DIMG_MEDIAN:
        shader = new Shader("src/shaders/median.vert", "src/shaders/median.frag");
        break;
    case DIMG_SOBEL_EDGE_DETECTION:
        shader = new Shader("src/shaders/sobel.vert", "src/shaders/sobel.frag");
        break;
    case DIMG_ROBERTS_EDGE_DETECTION:
        shader = new Shader("src/shaders/roberts.vert", "src/shaders/roberts.frag");
        break;
    case DIMG_PREWITT_EDGE_DETECTION:
        shader = new Shader("src/shaders/prewitt.vert", "src/shaders/prewitt.frag");
        break;
    case DIMG_LOG_EDGE_DETECTION:
        shader = new Shader("src/shaders/log.vert", "src/shaders/log.frag");
        break;
    case DIMG_TOON_SHADING:
        shader = new Shader("src/shaders/toonshading.vert", "src/shaders/toonshading.frag");
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

void dimg::setKernel(DIMGenum type, unsigned int &kernel, int kernelWidth, int kernelHeight) {
    std::vector<float> kernelData = std::vector<float>(49, 0.0f);
    // GPU Kernel
    switch (type) {
    case DIMG_MEAN_BLUR: {
        for (int i = 0; i < kernelHeight; i++)
            for (int j = 0; j < kernelWidth; j++)
                kernelData[i * kernelWidth + j] = 1;

        // Texture for mean blur
        glGenTextures(1, &kernel);
        glBindTexture(GL_TEXTURE_1D, kernel);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, kernelData.data());
        glBindTexture(GL_TEXTURE_1D, 0);
        break;
    }
    }
}

void dimg::setKernel(DIMGenum type, unsigned int& gx, unsigned int& gy, int kernelWidth, int kernelHeight) {
    // GPU Kernel
    switch (type) {
    case DIMG_SOBEL_EDGE_DETECTION: {
        std::vector<float> gxData = std::vector<float>(49, 0.0f);
        std::vector<float> gyData = std::vector<float>(49, 0.0f);
        glm::ivec2 pivot;
        int ki, kj, kiikjj;
        pivot.x = kernelHeight / 2;
        pivot.y = kernelWidth / 2;
        for (int i = 0; i < kernelHeight; i++) {
            for (int j = 0; j < kernelWidth; j++) {

                ki = i - pivot.x;
                kj = j - pivot.y;

                if (glm::ivec2(i, j) == pivot) {
                    gxData[i * kernelWidth + j] = 0;
                    gyData[i * kernelWidth + j] = 0;
                    continue;
                }
                // Gx_ij = i / (i*i + j*j)
                kiikjj = ki * ki + kj * kj;
                gxData[i * kernelWidth + j] = (float)ki / (float)kiikjj;
                gyData[i * kernelWidth + j] = (float)kj / (float)kiikjj;
            }
        }
        // Texture for sobel's gradient x-axis
        glGenTextures(1, &gx);
        glBindTexture(GL_TEXTURE_1D, gx);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, gxData.data());
        glBindTexture(GL_TEXTURE_1D, 0);

        // Texture for sobel's gradient y-axis
        glGenTextures(1, &gy);
        glBindTexture(GL_TEXTURE_1D, gy);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, gyData.data());
        glBindTexture(GL_TEXTURE_1D, 0);
        break;
    }
    case DIMG_ROBERTS_EDGE_DETECTION: {
        glm::ivec2 pivot;
        std::vector<float> gxData = std::vector<float>(49, 0.0f);
        std::vector<float> gyData = std::vector<float>(49, 0.0f);
        int val;
        pivot.x = kernelHeight / 2;
        pivot.y = kernelWidth / 2;
        val = pivot.x;
        for (int i = 0, j = 0; i < kernelHeight; i++, j++, val--) {
            if (!val)
                val--;
            gxData[i * kernelWidth + j] = (float)val;
        }

        val = pivot.y;
        for (int i = 0, j = kernelHeight - 1; i < kernelHeight; i++, j--, val--) {
            if (!val)
                val--;
            gyData[i * kernelWidth + j] = (float)val;
        }
        // Texture for robert's gradient x-axis
        glGenTextures(1, &gx);
        glBindTexture(GL_TEXTURE_1D, gx);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, gxData.data());
        glBindTexture(GL_TEXTURE_1D, 0);

        // Texture for robert's gradient y-axis
        glGenTextures(1, &gy);
        glBindTexture(GL_TEXTURE_1D, gy);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, gyData.data());
        glBindTexture(GL_TEXTURE_1D, 0);
        break;
    }
    case DIMG_PREWITT_EDGE_DETECTION: {
        std::vector<float> gxData = std::vector<float>(49, 0.0f);
        std::vector<float> gyData = std::vector<float>(49, 0.0f);
        glm::ivec2 pivot;
        int ki, kj;
        pivot.x = kernelHeight / 2;
        pivot.y = kernelWidth / 2;
        for (int i = 0; i < kernelHeight; i++) {
            for (int j = 0; j < kernelWidth; j++) {
                ki = i - pivot.x;
                kj = j - pivot.y;

                gxData[i * kernelWidth + j] = (float)ki;
                gyData[i * kernelWidth + j] = (float)kj;
            }
        }
        // Texture for prewitt's gradient x-axis
        glGenTextures(1, &gx);
        glBindTexture(GL_TEXTURE_1D, gx);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, gxData.data());
        glBindTexture(GL_TEXTURE_1D, 0);

        // Texture for prewitt's gradient y-axis
        glGenTextures(1, &gy);
        glBindTexture(GL_TEXTURE_1D, gy);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, gyData.data());
        glBindTexture(GL_TEXTURE_1D, 0);
        break;
    }
    }
}

void dimg::setKernel(DIMGenum type, unsigned int& gx, float sigma, int kernelWidth, int kernelHeight) {
    //GPU Kernel for LOG
    std::vector<float> gxData = std::vector<float>(49, 0.0f);
    glm::ivec2 pivot;
    pivot.x = kernelHeight / 2;
    pivot.y = kernelWidth  / 2;
    int ki, kj, kii, kjj;
    float sigma2 = 2 * sigma * sigma;
    float sigma4 = sigma * sigma * sigma * sigma;
    for (int i = 0; i < kernelHeight; i++) {
        for (int j = 0; j < kernelWidth; j++) {
            ki = i - pivot.x;
            kj = j - pivot.y;
            kii = ki * ki;
            kjj = kj * kj;
            gxData[i * kernelWidth + j] = (kii + kjj - sigma2) / sigma4 * exp(-(kii + kjj) / sigma2);
        }
    }
    // Texture for LoG's gradient x-axis
    glGenTextures(1, &gx);
    glBindTexture(GL_TEXTURE_1D, gx);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, gxData.data());
    glBindTexture(GL_TEXTURE_1D, 0);
}

void dimg::setKernel(DIMGenum type, std::vector<float> &kernel, int kernelWidth, int kernelHeight) {
    //CPU Kernel
    kernel = std::vector<float>(49, 0.0f);
    switch (type) {
    case DIMG_MEAN_BLUR:
        for (int i = 0; i < kernelHeight; i++)
            for (int j = 0; j < kernelWidth; j++)
                kernel[i * kernelWidth + j] = 1;
        break;
    }
}

void dimg::setKernel(DIMGenum type, std::vector<float> &gx, std::vector<float> &gy, int kernelWidth, int kernelHeight) {
    // CPU Kernel
    switch (type) {
    case DIMG_SOBEL_EDGE_DETECTION: {
        gx = std::vector<float>(49, 0.0f);
        gy = std::vector<float>(49, 0.0f);
        glm::ivec2 pivot;
        int ki, kj, kiikjj;
        pivot.x = kernelHeight / 2;
        pivot.y = kernelWidth / 2;
        for (int i = 0; i < kernelHeight; i++) {
            for (int j = 0; j < kernelWidth; j++) {

                ki = i - pivot.x;
                kj = j - pivot.y;

                if (glm::ivec2(i, j) == pivot) {
                    gx[i * kernelWidth + j] = 0;
                    gy[i * kernelWidth + j] = 0;
                    continue;
                }
                // Gx_ij = i / (i*i + j*j)
                kiikjj = ki * ki + kj * kj;
                gx[i * kernelWidth + j] = (float)ki / (float)kiikjj;
                gy[i * kernelWidth + j] = (float)kj / (float)kiikjj;
            }
        }
        break;
    }
    case DIMG_ROBERTS_EDGE_DETECTION: {
        glm::ivec2 pivot;
        gx = std::vector<float>(49, 0.0f);
        gy = std::vector<float>(49, 0.0f);
        int val;
        pivot.x = kernelHeight / 2;
        pivot.y = kernelWidth / 2;
        val = pivot.x;
        for (int i = 0, j = 0; i < kernelHeight; i++, j++, val--) {
            if (!val)
                val--;
            gx[i * kernelWidth + j] = (float)val;
        }

        val = pivot.y;
        for (int i = 0, j = kernelHeight - 1; i < kernelHeight; i++, j--, val--) {
            if (!val)
                val--;
            gy[i * kernelWidth + j] = (float)val;
        }
        break;
    }
    case DIMG_PREWITT_EDGE_DETECTION: {
        gx = std::vector<float>(49, 0.0f);
        gy = std::vector<float>(49, 0.0f);
        glm::ivec2 pivot;
        int ki, kj;
        pivot.x = kernelHeight / 2;
        pivot.y = kernelWidth / 2;
        for (int i = 0; i < kernelHeight; i++) {
            for (int j = 0; j < kernelWidth; j++) {
                ki = i - pivot.x;
                kj = j - pivot.y;

                gx[i * kernelWidth + j] = (float)ki;
                gy[i * kernelWidth + j] = (float)kj;
            }
        }
        break;
    }
    }
}

void dimg::setKernel(DIMGenum type, std::vector<float>& gx, float sigma, int kernelWidth, int kernelHeight) {
    //CPU Kernel for LOG
    gx = std::vector<float>(49, 0.0f);
    glm::ivec2 pivot;
    pivot.x = kernelHeight / 2;
    pivot.y = kernelWidth / 2;
    int ki, kj, kii, kjj;
    float sigma2 = 2 * sigma * sigma;
    float sigma4 = sigma * sigma * sigma * sigma;
    for (int i = 0; i < kernelHeight; i++) {
        for (int j = 0; j < kernelWidth; j++) {
            ki = i - pivot.x;
            kj = j - pivot.y;
            kii = ki * ki;
            kjj = kj * kj;
            gx[i * kernelWidth + j] = (kii + kjj - sigma2) / sigma4 * exp(-(kii + kjj) / sigma2);
        }
    }
}

bool dimg::isInImage(int x, int y, int width, int height) {
    return x < 0 || x >= (height - 1) * width * 3 || y < 0 || y >= width * 3 ? false : true;
}

void dimg::sort(std::vector<glm::ivec3>& array) {
    glm::vec3 temp;
    for (unsigned int i = 0; i < array.size(); i++)
        for (unsigned int j = 0; j < array.size(); j++)
            if (array[i].r > array[j].r) {
                temp = array[j];
                array[i] = array[j];
                array[j] = temp;
            }
}

void dimg::initImage(IMGDATA &dest, unsigned char* data, int width, int height, int numOfChannels, const char* path){
    dest.data = new unsigned char[height * width * 3];
    dest.height = height;
    dest.width = width;
    dest.numOfChannels = numOfChannels;
    dest.path = path;
}

bool dimg::dimg_negative(const char* target, const char* dest, DIMGenum hwAcc) {
    // Load image information
    IMGDATA input_img;
    if (!loadImage(target, input_img))
        return false;
    // Output image init
    IMGDATA output_img;
    initImage(output_img, input_img.data, input_img.width, input_img.height, input_img.numOfChannels, dest);
    // Index (GPU) of target texture
    unsigned int texID;
    // Create framebuffer
    setFrameBuffer(output_img.width, output_img.height);
    // Set viewport size to render in original resolution
    glViewport(0, 0, output_img.width, output_img.height);
    // GPU Implementation
    if (hwAcc == DIMG_HARDWARE_ACCELERATION) {
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
        // Deletes texture from gpu
        glDeleteTextures(1, &imageID);
        // Deletes texture from gpu
        glDeleteTextures(1, &texID);
    }
    // CPU Implementation
    else {
        for (int i = 0; i < output_img.height * output_img.width * 3; i++)
            output_img.data[i] = 255 - input_img.data[i];
    }
    // Save image 
    if (!saveImage(output_img))
        return false;
    // Free image bytes on memory
    delete output_img.data;
    // Free image bytes on memory
    delete input_img.data;

    return true;
}

bool dimg::dimg_grayscale(const char* target, const char* dest, DIMGenum hwAcc) {
    // Load image information
    IMGDATA input_img;
    if (!loadImage(target, input_img))
        return false;
    // Output image init
    IMGDATA output_img;
    initImage(output_img, input_img.data, input_img.width, input_img.height, input_img.numOfChannels, dest);
    // Index (GPU) of target texture
    unsigned int texID;
    // Create framebuffer
    setFrameBuffer(output_img.width, output_img.height);
    // Set viewport size to render in original resolution
    glViewport(0, 0, output_img.width, output_img.height);
   
    // GPU Implementation
    if (hwAcc == DIMG_HARDWARE_ACCELERATION) {
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
        // Deletes texture from gpu
        glDeleteTextures(1, &imageID);
        // Deletes texture from gpu
        glDeleteTextures(1, &texID);
    }
    // CPU Implementation
    else {
        int gs_value;
        for (int i = 0; i < input_img.height * input_img.width * 3; i+=3) {

            gs_value = (int)((input_img.data[i] * 0.2989) +
                         (input_img.data[i + 1] * 0.5870) +
                         (input_img.data[i + 2] * 0.1140));
            // Red
            output_img.data[i + 0] = gs_value;
            // Green
            output_img.data[i + 1] = gs_value;
            // Blue
            output_img.data[i + 2] = gs_value;
        }
    }
    // Save image 
    if (!saveImage(output_img))
        return false;
    // Free image bytes on memory
    delete output_img.data;
    // Free image bytes on memory
    delete input_img.data;

    return true;
}

bool dimg::dimg_black_and_white(const char* target, const char* dest, int threshold, DIMGenum hwAcc) {
    // Load image information
    IMGDATA input_img;
    if (!loadImage(target, input_img))
        return false;
    // Output image init
    IMGDATA output_img;
    initImage(output_img, input_img.data, input_img.width, input_img.height, input_img.numOfChannels, dest);
    // Index (GPU) of target texture
    unsigned int texID;
    // Create framebuffer
    setFrameBuffer(output_img.width, output_img.height);
    // Set viewport size to render in original resolution
    glViewport(0, 0, output_img.width, output_img.height);
    
    // GPU Implementation
    if (hwAcc == DIMG_HARDWARE_ACCELERATION) {
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
        // Deletes texture from gpu
        glDeleteTextures(1, &imageID);
        // Deletes texture from gpu
        glDeleteTextures(1, &texID);
    }
    // CPU Implementation
    else {
        int gs_value, bnw;
        for (int i = 0; i < input_img.height * input_img.width * 3; i += 3) {
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

    // Free image bytes on memory
    delete output_img.data;
    // Free image bytes on memory
    delete input_img.data;

    return true;
}

bool dimg::dimg_mean_blur(const char* target, const char* dest, int kernelWidth, int kernelHeight, DIMGenum hwAcc) {
    // Load image information
    IMGDATA input_img;
    if (!loadImage(target, input_img))
        return false;
    // Output image init
    IMGDATA output_img;
    initImage(output_img, input_img.data, input_img.width, input_img.height, input_img.numOfChannels, dest);
    // Index (GPU) of target texture
    unsigned int texID;
    // Create framebuffer
    setFrameBuffer(output_img.width, output_img.height);
    // Set viewport size to render in original resolution
    glViewport(0, 0, output_img.width, output_img.height);
    // GPU Implementation
    if (hwAcc == DIMG_HARDWARE_ACCELERATION) {
        unsigned int kernel;
        // Compute Kernel
        setKernel(DIMG_MEAN_BLUR,kernel, kernelWidth, kernelHeight);
        // Create GPU Texture
        if (!createTexture2D(input_img, texID))
            return false;
        // Set shader configuration
        shader = setShader(DIMG_MEAN_BLUR);
        shader->setInt("kernel", 1);
        shader->setInt("kWidth", kernelWidth);
        shader->setInt("kHeight", kernelHeight);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_1D, kernel);
        // Deferred shading
        deferredShading(texID);
        // Get image data from GPU
        glBindTexture(GL_TEXTURE_2D, imageID);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, output_img.data);
        // Deletes texture from gpu
        glDeleteTextures(1, &imageID);
        // Deletes texture from gpu
        glDeleteTextures(1, &texID);
    }
    // CPU Implementation
    else {
        std::vector<float> kernel;
        glm::ivec3 mean;
        glm::ivec2 pivot = glm::ivec2(kernelHeight / 2, kernelWidth / 2);
        glm::ivec2 img_pos, k_pos;
        int totalSize = kernelHeight * kernelWidth;
        int rgb_offset;
        setKernel(DIMG_MEAN_BLUR,kernel, kernelWidth, kernelHeight);
        for (int i = 0; i < input_img.height; i++) {
            for (int j = 0; j < input_img.width * 3; j += 3) {
                mean = glm::ivec3(0);
                img_pos = glm::ivec2(i * input_img.width * 3, j);
                // Kernel Loop
                for (int ii = 0; ii < kernelHeight; ii++) {
                    //printf("--------------------- KROW: %i\n", ii);
                    for (int jj = 0; jj < kernelWidth; jj++) {
                        rgb_offset = (jj - pivot.y) * 3;
                        k_pos = glm::ivec2(img_pos.x + (ii - pivot.x) * input_img.width * 3, img_pos.y + rgb_offset);
                        if (!isInImage(k_pos.x, k_pos.y, input_img.width, input_img.height))
                            continue;
                        // red
                        mean.r += input_img.data[k_pos.x + (k_pos.y + 0)] * (int)kernel[ii * kernelWidth + jj];
                        // green
                        mean.g += input_img.data[k_pos.x + (k_pos.y + 1)] * (int)kernel[ii * kernelWidth + jj];
                        // blue
                        mean.b += input_img.data[k_pos.x + (k_pos.y + 2)] * (int)kernel[ii * kernelWidth + jj];
                    }
                }
                mean.r = mean.r / totalSize;
                mean.g = mean.g / totalSize;
                mean.b = mean.b / totalSize;

                output_img.data[i * input_img.width * 3 + (j + 0)] = mean.r;
                output_img.data[i * input_img.width * 3 + (j + 1)] = mean.g;
                output_img.data[i * input_img.width * 3 + (j + 2)] = mean.b;
            }
        }

    }
    // Save image 
    if (!saveImage(output_img))
        return false;

    // Free image bytes on memory
    delete output_img.data;
    // Free image bytes on memory
    delete input_img.data;

    return true;
}

bool dimg::dimg_median(const char* target, const char* dest, int kernelWidth, int kernelHeight, DIMGenum hwAcc) {
    // Load image information
    IMGDATA input_img;
    if (!loadImage(target, input_img))
        return false;
    // Output image init
    IMGDATA output_img;
    initImage(output_img, input_img.data, input_img.width, input_img.height, input_img.numOfChannels, dest);
    // Index (GPU) of target texture
    unsigned int texID;
    // Create framebuffer
    setFrameBuffer(output_img.width, output_img.height);
    // Set viewport size to render in original resolution
    glViewport(0, 0, output_img.width, output_img.height);
    // GPU Implementation
    if (hwAcc == DIMG_HARDWARE_ACCELERATION) {
        // Create GPU Texture
        if (!createTexture2D(input_img, texID))
            return false;
        // Set shader configuration
        shader = setShader(DIMG_MEDIAN);
        shader->setInt("kWidth", kernelWidth);
        shader->setInt("kHeight", kernelHeight);
        // Deferred shading
        deferredShading(texID);
        // Get image data from GPU
        glBindTexture(GL_TEXTURE_2D, imageID);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, output_img.data);
        // Deletes texture from gpu
        glDeleteTextures(1, &imageID);
        // Deletes texture from gpu
        glDeleteTextures(1, &texID);
    }
    // CPU Implementation
    else {
        std::vector<glm::ivec3> rgb_array;
        glm::ivec2 pivot = glm::ivec2(kernelHeight / 2, kernelWidth / 2);
        glm::ivec2 img_pos, k_pos;
        int totalSize = kernelHeight * kernelWidth;
        int rgb_offset;
        for (int i = 0; i < input_img.height; i++) {
            for (int j = 0; j < input_img.width * 3; j += 3) {
                img_pos = glm::ivec2(i * input_img.width * 3, j);
                // Kernel Loop
                for (int ii = 0; ii < kernelHeight; ii++) {
                    //printf("--------------------- KROW: %i\n", ii);
                    for (int jj = 0; jj < kernelWidth; jj++) {
                        rgb_offset = (jj - pivot.y) * 3;
                        k_pos = glm::ivec2(img_pos.x + (ii - pivot.x) * input_img.width * 3, img_pos.y + rgb_offset);
                        if (!isInImage(k_pos.x, k_pos.y, input_img.width, input_img.height))
                            continue;
                        rgb_array.push_back(glm::ivec3(
                            input_img.data[k_pos.x + (k_pos.y + 0)],
                            input_img.data[k_pos.x + (k_pos.y + 1)],
                            input_img.data[k_pos.x + (k_pos.y + 2)]
                        ));
                    }
                }
                // Sort colors
                sort(rgb_array);
                // Assign median color to output image
                output_img.data[i * input_img.width * 3 + (j + 0)] = rgb_array[rgb_array.size() / 2].r;
                output_img.data[i * input_img.width * 3 + (j + 1)] = rgb_array[rgb_array.size() / 2].g;
                output_img.data[i * input_img.width * 3 + (j + 2)] = rgb_array[rgb_array.size() / 2].b;
                rgb_array.clear();
            }
        }
    }
    // Save image 
    if (!saveImage(output_img))
        return false;

    // Free image bytes on memory
    delete output_img.data;
    // Free image bytes on memory
    delete input_img.data;

    return true;
}

bool dimg::dimg_sobel_edge_detection(const char* target, const char* dest, int kernelWidth, int kernelHeight, DIMGenum hwAcc) {
    // Load image information
    IMGDATA input_img;
    if (!loadImage(target, input_img))
        return false;
    // Output image init
    IMGDATA output_img;
    initImage(output_img, input_img.data, input_img.width, input_img.height, input_img.numOfChannels, dest);
    // Index (GPU) of target texture
    unsigned int texID;
    // Create framebuffer
    setFrameBuffer(output_img.width, output_img.height);
    // Set viewport size to render in original resolution
    glViewport(0, 0, output_img.width, output_img.height);
    // GPU Implementation
    if (hwAcc == DIMG_HARDWARE_ACCELERATION) {
        unsigned int gx,gy;
        // Compute Kernel
        setKernel(DIMG_SOBEL_EDGE_DETECTION, gx, gy, kernelWidth, kernelHeight);
        // Create GPU Texture
        if (!createTexture2D(input_img, texID))
            return false;
        // Set shader configuration
        shader = setShader(DIMG_SOBEL_EDGE_DETECTION);
        shader->setInt("kWidth", kernelWidth);
        shader->setInt("kHeight", kernelHeight);
        shader->setInt("gx", 1);
        shader->setInt("gy", 2);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_1D, gx);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_1D, gy);
        // Deferred shading
        deferredShading(texID);
        // Get image data from GPU
        glBindTexture(GL_TEXTURE_2D, imageID);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, output_img.data);
        // Deletes texture from gpu
        glDeleteTextures(1, &imageID);
        // Deletes texture from gpu
        glDeleteTextures(1, &texID);
    }
    // CPU Implementation
    else {
        std::vector<float> gx, gy;
        glm::ivec3 sobel;
        glm::vec3 s_gx, s_gy;
        glm::ivec2 pivot = glm::ivec2(kernelHeight / 2, kernelWidth / 2);
        glm::ivec2 img_pos, k_pos;
        int totalSize = kernelHeight * kernelWidth;
        int rgb_offset, gs_value;

        // Compute kernel
        setKernel(DIMG_SOBEL_EDGE_DETECTION, gx, gy, kernelWidth, kernelHeight);

        for (int i = 0; i < input_img.height; i++) {
            for (int j = 0; j < input_img.width * 3; j += 3) {
                sobel = glm::ivec3(0);
                s_gx  = glm::vec3(0);
                s_gy  = glm::vec3(0);
                img_pos = glm::ivec2(i * input_img.width * 3, j);
                // Kernel Loop
                for (int ii = 0; ii < kernelHeight; ii++) {
                    //printf("--------------------- KROW: %i\n", ii);
                    for (int jj = 0; jj < kernelWidth; jj++) {
                        rgb_offset = (jj - pivot.y) * 3;
                        k_pos = glm::ivec2(img_pos.x + (ii - pivot.x) * input_img.width * 3, img_pos.y + rgb_offset);
                        if (!isInImage(k_pos.x, k_pos.y, input_img.width, input_img.height))
                            continue;
                        // X-axis gradient
                        s_gx += glm::vec3(
                            input_img.data[k_pos.x + (k_pos.y + 0)] * gx[ii * kernelWidth + jj],
                            input_img.data[k_pos.x + (k_pos.y + 1)] * gx[ii * kernelWidth + jj],
                            input_img.data[k_pos.x + (k_pos.y + 2)] * gx[ii * kernelWidth + jj]);
                        // Y-axis gradient
                        s_gy += glm::vec3(
                            input_img.data[k_pos.x + (k_pos.y + 0)] * gy[ii * kernelWidth + jj],
                            input_img.data[k_pos.x + (k_pos.y + 1)] * gy[ii * kernelWidth + jj],
                            input_img.data[k_pos.x + (k_pos.y + 2)] * gy[ii * kernelWidth + jj]);
                    }
                }
                // Compute gradient's magnitude
                sobel = glm::round(glm::sqrt(s_gx * s_gx + s_gy * s_gy));
                gs_value =  (int)(sobel.r * 0.2989) +
                            (int)(sobel.g * 0.5870) +
                            (int)(sobel.b * 0.1140);
                sobel = glm::ivec3(gs_value, gs_value, gs_value);
                // Assign median color to output image
                output_img.data[i * input_img.width * 3 + (j + 0)] = sobel.r;
                output_img.data[i * input_img.width * 3 + (j + 1)] = sobel.g;
                output_img.data[i * input_img.width * 3 + (j + 2)] = sobel.b;
            }
        }
    }
    // Save image 
    if (!saveImage(output_img))
        return false;

    // Free image bytes on memory
    delete output_img.data;
    // Free image bytes on memory
    delete input_img.data;

    return true;
}

bool dimg::dimg_roberts_edge_detection(const char* target, const char* dest, int kernelWidth, int kernelHeight, DIMGenum hwAcc) {
    // Load image information
    IMGDATA input_img;
    if (!loadImage(target, input_img))
        return false;
    // Output image init
    IMGDATA output_img;
    initImage(output_img, input_img.data, input_img.width, input_img.height, input_img.numOfChannels, dest);
    // Index (GPU) of target texture
    unsigned int texID;
    // Create framebuffer
    setFrameBuffer(output_img.width, output_img.height);
    // Set viewport size to render in original resolution
    glViewport(0, 0, output_img.width, output_img.height);
    // GPU Implementation
    if (hwAcc == DIMG_HARDWARE_ACCELERATION) {
        unsigned int gx, gy;
        // Compute Kernel
        setKernel(DIMG_ROBERTS_EDGE_DETECTION, gx, gy, kernelWidth, kernelHeight);
        // Create GPU Texture
        if (!createTexture2D(input_img, texID))
            return false;
        // Set shader configuration
        shader = setShader(DIMG_ROBERTS_EDGE_DETECTION);
        shader->setInt("kWidth", kernelWidth);
        shader->setInt("kHeight", kernelHeight);
        shader->setInt("gx", 1);
        shader->setInt("gy", 2);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_1D, gx);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_1D, gy);
        // Deferred shading
        deferredShading(texID);
        // Get image data from GPU
        glBindTexture(GL_TEXTURE_2D, imageID);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, output_img.data);
        // Deletes texture from gpu
        glDeleteTextures(1, &imageID);
        // Deletes texture from gpu
        glDeleteTextures(1, &texID);
    }
    // CPU Implementation
    else {
        std::vector<float> gx, gy;
        glm::ivec3 roberts;
        glm::vec3 r_gx, r_gy;
        glm::ivec2 pivot = glm::ivec2(kernelHeight / 2, kernelWidth / 2);
        glm::ivec2 img_pos, k_pos;
        int totalSize = kernelHeight * kernelWidth;
        int rgb_offset, gs_value;

        // Compute kernel
        setKernel(DIMG_ROBERTS_EDGE_DETECTION, gx, gy, kernelWidth, kernelHeight);

        for (int i = 0; i < input_img.height; i++) {
            for (int j = 0; j < input_img.width * 3; j += 3) {
                roberts = glm::ivec3(0);
                r_gx = glm::vec3(0);
                r_gy = glm::vec3(0);
                img_pos = glm::ivec2(i * input_img.width * 3, j);
                // Kernel Loop
                for (int ii = 0; ii < kernelHeight; ii++) {
                    //printf("--------------------- KROW: %i\n", ii);
                    for (int jj = 0; jj < kernelWidth; jj++) {
                        rgb_offset = (jj - pivot.y) * 3;
                        k_pos = glm::ivec2(img_pos.x + (ii - pivot.x) * input_img.width * 3, img_pos.y + rgb_offset);
                        if (!isInImage(k_pos.x, k_pos.y, input_img.width, input_img.height))
                            continue;
                        // X-axis gradient
                        r_gx += glm::vec3(
                            input_img.data[k_pos.x + (k_pos.y + 0)] * gx[ii * kernelWidth + jj],
                            input_img.data[k_pos.x + (k_pos.y + 1)] * gx[ii * kernelWidth + jj],
                            input_img.data[k_pos.x + (k_pos.y + 2)] * gx[ii * kernelWidth + jj]);
                        // Y-axis gradient
                        r_gy += glm::vec3(
                            input_img.data[k_pos.x + (k_pos.y + 0)] * gy[ii * kernelWidth + jj],
                            input_img.data[k_pos.x + (k_pos.y + 1)] * gy[ii * kernelWidth + jj],
                            input_img.data[k_pos.x + (k_pos.y + 2)] * gy[ii * kernelWidth + jj]);
                    }
                }
                // Compute gradient's magnitude
                roberts = glm::round(glm::sqrt(r_gx * r_gx + r_gy * r_gy));
                gs_value = (int)(roberts.r * 0.2989) +
                           (int)(roberts.g * 0.5870) +
                           (int)(roberts.b * 0.1140);
                roberts = glm::ivec3(gs_value, gs_value, gs_value);
                // Assign median color to output image
                output_img.data[i * input_img.width * 3 + (j + 0)] = roberts.r;
                output_img.data[i * input_img.width * 3 + (j + 1)] = roberts.g;
                output_img.data[i * input_img.width * 3 + (j + 2)] = roberts.b;
            }
        }
    }
    // Save image 
    if (!saveImage(output_img))
        return false;

    // Free image bytes on memory
    delete output_img.data;
    // Free image bytes on memory
    delete input_img.data;

    return true;
}

bool dimg::dimg_prewitt_edge_detection(const char* target, const char* dest, int kernelWidth, int kernelHeight, DIMGenum hwAcc) {
    // Load image information
    IMGDATA input_img;
    if (!loadImage(target, input_img))
        return false;
    // Output image init
    IMGDATA output_img;
    initImage(output_img, input_img.data, input_img.width, input_img.height, input_img.numOfChannels, dest);
    // Index (GPU) of target texture
    unsigned int texID;
    // Create framebuffer
    setFrameBuffer(output_img.width, output_img.height);
    // Set viewport size to render in original resolution
    glViewport(0, 0, output_img.width, output_img.height);
    // GPU Implementation
    if (hwAcc == DIMG_HARDWARE_ACCELERATION) {
        unsigned int gx, gy;
        // Compute Kernel
        setKernel(DIMG_PREWITT_EDGE_DETECTION, gx, gy, kernelWidth, kernelHeight);
        // Create GPU Texture
        if (!createTexture2D(input_img, texID))
            return false;
        // Set shader configuration
        shader = setShader(DIMG_PREWITT_EDGE_DETECTION);
        shader->setInt("kWidth", kernelWidth);
        shader->setInt("kHeight", kernelHeight);
        shader->setInt("gx", 1);
        shader->setInt("gy", 2);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_1D, gx);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_1D, gy);
        // Deferred shading
        deferredShading(texID);
        // Get image data from GPU
        glBindTexture(GL_TEXTURE_2D, imageID);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, output_img.data);
        // Deletes texture from gpu
        glDeleteTextures(1, &imageID);
        // Deletes texture from gpu
        glDeleteTextures(1, &texID);
    }
    // CPU Implementation
    else {
        std::vector<float> gx, gy;
        glm::ivec3 prewitt;
        glm::vec3 p_gx, p_gy;
        glm::ivec2 pivot = glm::ivec2(kernelHeight / 2, kernelWidth / 2);
        glm::ivec2 img_pos, k_pos;
        int totalSize = kernelHeight * kernelWidth;
        int rgb_offset, gs_value;

        // Compute kernel
        setKernel(DIMG_PREWITT_EDGE_DETECTION, gx, gy, kernelWidth, kernelHeight);

        for (int i = 0; i < input_img.height; i++) {
            for (int j = 0; j < input_img.width * 3; j += 3) {
                prewitt = glm::ivec3(0);
                p_gx = glm::ivec3(0);
                p_gy = glm::ivec3(0);
                img_pos = glm::ivec2(i * input_img.width * 3, j);
                // Kernel Loop
                for (int ii = 0; ii < kernelHeight; ii++) {
                    //printf("--------------------- KROW: %i\n", ii);
                    for (int jj = 0; jj < kernelWidth; jj++) {
                        rgb_offset = (jj - pivot.y) * 3;
                        k_pos = glm::ivec2(img_pos.x + (ii - pivot.x) * input_img.width * 3, img_pos.y + rgb_offset);
                        if (!isInImage(k_pos.x, k_pos.y, input_img.width, input_img.height))
                            continue;
                        // X-axis gradient
                        p_gx += glm::vec3(
                            input_img.data[k_pos.x + (k_pos.y + 0)] * gx[ii * kernelWidth + jj],
                            input_img.data[k_pos.x + (k_pos.y + 1)] * gx[ii * kernelWidth + jj],
                            input_img.data[k_pos.x + (k_pos.y + 2)] * gx[ii * kernelWidth + jj]);
                        // Y-axis gradient
                        p_gy += glm::vec3(
                            input_img.data[k_pos.x + (k_pos.y + 0)] * gy[ii * kernelWidth + jj],
                            input_img.data[k_pos.x + (k_pos.y + 1)] * gy[ii * kernelWidth + jj],
                            input_img.data[k_pos.x + (k_pos.y + 2)] * gy[ii * kernelWidth + jj]);
                    }
                }
                // Compute gradient's magnitude
                prewitt = glm::round(glm::sqrt(p_gx * p_gx + p_gy * p_gy));
                gs_value = (int)(prewitt.r * 0.2989) +
                           (int)(prewitt.g * 0.5870) +
                           (int)(prewitt.b * 0.1140);
                prewitt = glm::ivec3(gs_value, gs_value, gs_value);
                // Assign median color to output image
                output_img.data[i * input_img.width * 3 + (j + 0)] = prewitt.r;
                output_img.data[i * input_img.width * 3 + (j + 1)] = prewitt.g;
                output_img.data[i * input_img.width * 3 + (j + 2)] = prewitt.b;
            }
        }
    }
    // Save image 
    if (!saveImage(output_img))
        return false;

    // Free image bytes on memory
    delete output_img.data;
    // Free image bytes on memory
    delete input_img.data;

    return true;
}

bool dimg::dimg_log_edge_detection(const char* target, const char* dest, int kernelWidth, int kernelHeight, float sigma, DIMGenum hwAcc) {
    // Load image information
    IMGDATA input_img;
    if (!loadImage(target, input_img))
        return false;
    // Output image init
    IMGDATA output_img;
    initImage(output_img, input_img.data, input_img.width, input_img.height, input_img.numOfChannels, dest);
    // Index (GPU) of target texture
    unsigned int texID;
    // Create framebuffer
    setFrameBuffer(output_img.width, output_img.height);
    // Set viewport size to render in original resolution
    glViewport(0, 0, output_img.width, output_img.height);
    // GPU Implementation
    if (hwAcc == DIMG_HARDWARE_ACCELERATION) {
        unsigned int gx;
        // Compute Kernel
        setKernel(DIMG_LOG_EDGE_DETECTION, gx, sigma, kernelWidth, kernelHeight);
        // Create GPU Texture
        if (!createTexture2D(input_img, texID))
            return false;
        // Set shader configuration
        shader = setShader(DIMG_LOG_EDGE_DETECTION);
        shader->setInt("kWidth", kernelWidth);
        shader->setInt("kHeight", kernelHeight);
        shader->setInt("gx", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_1D, gx);
        // Deferred shading
        deferredShading(texID);
        // Get image data from GPU
        glBindTexture(GL_TEXTURE_2D, imageID);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, output_img.data);
        // Deletes texture from gpu
        glDeleteTextures(1, &imageID);
        // Deletes texture from gpu
        glDeleteTextures(1, &texID);
    }
    // CPU Implementation
    else {
        std::vector<float> gx;
        glm::ivec3 log;
        glm::vec3 log_gx;
        glm::ivec2 pivot = glm::ivec2(kernelHeight / 2, kernelWidth / 2);
        glm::ivec2 img_pos, k_pos;
        int totalSize = kernelHeight * kernelWidth;
        int rgb_offset, gs_value;

        // Compute kernel
        setKernel(DIMG_LOG_EDGE_DETECTION, gx, sigma, kernelWidth, kernelHeight);

        for (int i = 0; i < input_img.height; i++) {
            for (int j = 0; j < input_img.width * 3; j += 3) {
                log = glm::ivec3(0);
                log_gx = glm::ivec3(0);
                img_pos = glm::ivec2(i * input_img.width * 3, j);
                // Kernel Loop
                for (int ii = 0; ii < kernelHeight; ii++) {
                    //printf("--------------------- KROW: %i\n", ii);
                    for (int jj = 0; jj < kernelWidth; jj++) {
                        rgb_offset = (jj - pivot.y) * 3;
                        k_pos = glm::ivec2(img_pos.x + (ii - pivot.x) * input_img.width * 3, img_pos.y + rgb_offset);
                        if (!isInImage(k_pos.x, k_pos.y, input_img.width, input_img.height))
                            continue;
                        // X-axis gradient
                        log_gx += glm::vec3(
                            input_img.data[k_pos.x + (k_pos.y + 0)] * gx[ii * kernelWidth + jj],
                            input_img.data[k_pos.x + (k_pos.y + 1)] * gx[ii * kernelWidth + jj],
                            input_img.data[k_pos.x + (k_pos.y + 2)] * gx[ii * kernelWidth + jj]);
                    }
                }
                // Compute gradient's grayscale
                gs_value = (int)(log_gx.r * 0.2989) +
                           (int)(log_gx.g * 0.5870) +
                           (int)(log_gx.b * 0.1140);
                log = glm::ivec3(gs_value, gs_value, gs_value);
                // Assign median color to output image
                output_img.data[i * input_img.width * 3 + (j + 0)] = log.r;
                output_img.data[i * input_img.width * 3 + (j + 1)] = log.g;
                output_img.data[i * input_img.width * 3 + (j + 2)] = log.b;
            }
        }
    }
    // Save image 
    if (!saveImage(output_img))
        return false;

    // Free image bytes on memory
    delete output_img.data;
    // Free image bytes on memory
    delete input_img.data;

    return true;
}

bool dimg::dimg_toon_shading(const char* target, const char* dest, int borderRadius, int colorDiscretization, DIMGenum hwAcc) {
    // Load image information
    IMGDATA input_img;
    if (!loadImage(target, input_img))
        return false;
    // Output image init
    IMGDATA output_img;
    initImage(output_img, input_img.data, input_img.width, input_img.height, input_img.numOfChannels, dest);
    // Index (GPU) of target texture
    unsigned int texID;
    // Create framebuffer
    setFrameBuffer(output_img.width, output_img.height);
    // Set viewport size to render in original resolution
    glViewport(0, 0, output_img.width, output_img.height);

    // GPU Implementation
    if (hwAcc == DIMG_HARDWARE_ACCELERATION){
        unsigned int gx, gy;
        // Compute Sobel kernel
        setKernel(DIMG_SOBEL_EDGE_DETECTION, gx, gy, borderRadius, borderRadius);
        // Create target GPU Texture
        if (!createTexture2D(input_img, texID))
            return false;
        // Set shader configuration
        shader = setShader(DIMG_TOON_SHADING);
        shader->setInt("borderRadius", borderRadius);
        shader->setInt("medianRadius", colorDiscretization);
        shader->setInt("gx", 1);
        shader->setInt("gy", 2);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_1D, gx);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_1D, gy);
        // Deferred shading
        deferredShading(texID);
        // Get image data from GPU
        glBindTexture(GL_TEXTURE_2D, imageID);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, output_img.data);
        // Deletes texture from gpu
        glDeleteTextures(1, &imageID);
        // Deletes texture from gpu
        glDeleteTextures(1, &texID);
    }
    // CPU Implementation
    else {

    }
    // Save image 
    if (!saveImage(output_img))
        return false;

    // Free image bytes on memory
    delete output_img.data;
    // Free image bytes on memory
    delete input_img.data;

    return true;
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
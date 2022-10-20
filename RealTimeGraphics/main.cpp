#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height); 
void processInput(GLFWwindow* window);
std::string fileToString(const char* filePath);
unsigned int fileToShader(GLenum shaderType, const char* filePath);

int main()
{
    float windowHeight{ 900.f };
    float windowWidth{ 1200.f };

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "RealTimeGraphics", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    unsigned int baseShaderProgram;
    baseShaderProgram = glCreateProgram();

    unsigned int baseVShader = fileToShader(GL_VERTEX_SHADER, "basicVS.glsl");
    unsigned int baseFShader = fileToShader(GL_FRAGMENT_SHADER, "basicFS.glsl");

    glAttachShader(baseShaderProgram, baseVShader);
    glAttachShader(baseShaderProgram, baseFShader);
    glLinkProgram(baseShaderProgram);

    glDeleteShader(baseVShader);
    glDeleteShader(baseFShader);

    glUseProgram(baseShaderProgram);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        //INPUT
        processInput(window);

        //RENDER
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(baseShaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //BUFFERS
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

std::string fileToString(const char* filePath) //converted from opengl site
{
    std::ifstream fp;
    fp.open(filePath, std::ifstream::in);
    if (fp.is_open() == false) {
        return "";
    }
    std::stringstream ss;
    ss << fp.rdbuf();
    return ss.str();

    //std::string outString;
    //std::ifstream file;

    //file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    //try
    //{
    //    // open files
    //    file.open(filePath);
    //    std::stringstream fileStream;
    //    // read file's buffer contents into streams
    //    fileStream << file.rdbuf();
    //    // close file handlers
    //    file.close();
    //    // convert stream into string
    //    outString = fileStream.str();
    //}
    //catch (std::ifstream::failure e)
    //{
    //    std::cout << "ERROR_FILE_NOT_SUCCESFULLY_READ" << std::endl;
    //}

    //return outString;
}


unsigned int fileToShader( GLenum shaderType, const char* filePath)
{
    unsigned int shader = glCreateShader(shaderType);
    std::string shaderString = fileToString(filePath);
    const char* shaderSource{ shaderString.c_str() };
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    //error checking
    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "SHADER_COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}
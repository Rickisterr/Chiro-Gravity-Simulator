// For linux, run run_simulator.sh and for windows powershell, run run_simulator.ps1
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../include/Models.h"


// Default options values at starting
float FoV = 45;
float nearClippingVal = 0.5f;                                           // Too small causes precision issues
float farClippingVal = 100.0f;                                          // Too big requires too much computation
glm::vec3 cameraPosn = glm::vec3(0.0f, 0.0f, 0.0f);                     // camera position
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);                   // front direction of camera
glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);                       // direction of camera's z axis
const float cameraSpeed = 0.2f;

const char* vertexShaderScript = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 Posn;
    uniform mat4 Model;
    uniform mat4 View;
    uniform mat4 Perspective;
    void main() {
        gl_Position = Perspective * View * Model * vec4(Posn, 1.0);
    }
)glsl";

const char* fragmentShaderScript = R"glsl(
    #version 330 core
    out vec4 FragColor;
    uniform vec4 currentColor;
    void main() {
        FragColor = currentColor;
    }
)glsl";

void processWindowCloseInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    float aspectRatio = (float)width / (float)height;

    // Update projection matrix
    glm::mat4 Perspective = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
}

void processWindowCloseInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {

    // Update cameraPosn View
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPosn += cameraSpeed * glm::normalize(glm::cross(cameraFront, upVector));
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPosn -= cameraSpeed * glm::normalize(glm::cross(cameraFront, upVector));
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPosn += cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPosn -= cameraSpeed * cameraFront;
    }
}

std::vector<Body> InitializeModels(GLuint shader) {
    std::vector<Body> bodies;

    // Planet 1
    float radius1 = 1.5f;
    Body planet1(radius1, glm::vec3(0.0f, 0.0f, 0.5f), {1.0f, 0.0f, 0.0f, 1.0f}, shader);
    bodies.push_back(planet1);

    // Planet 2
    float radius2 = 0.5f;
    Body planet2(radius2, glm::vec3(-1.0f, -1.0f, -0.5f), {1.0f, 0.0f, 1.0f, 1.0f}, shader);
    bodies.push_back(planet2);

    return bodies;
}

void DrawModels(std::vector<Body> bodies) {
    int size = bodies.size();
    
    for (int idx=0; idx<size; idx++) {
        bodies[idx].draw_body();
    }

    return;
}




// MAIN PROGRAM
int main() {

    // Starting OpenGL
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Creating window
    GLFWwindow* window = glfwCreateWindow(800, 600, "3D Gravity Simulator", NULL, NULL);

    if (window == NULL){
        printf("Failed to create window\n");
        glfwTerminate();

        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    // In case of window resizing, change Viewport size
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Loading and linking shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderScript, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Vertex Shader Compilation Failed: %s \n", infoLog);
    }

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragmentShaderScript, NULL);
    glCompileShader(fragShader);

    int success2;
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success2);
    if (!success)
    {
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        printf("Fragment Shader Compilation Failed: %s \n", infoLog);
    }

    GLuint shader = glCreateProgram();
    glAttachShader(shader, vertexShader);
    glAttachShader(shader, fragShader);
    glLinkProgram(shader);

    // Deleting shaders after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);

    // Initializing Models
    std::vector<Body> bodies = InitializeModels(shader);

    // Using shader program
    glUseProgram(shader);
    glEnable(GL_DEPTH_TEST);

    // Transformation matrices involved in 3D
    glm::mat4 View = glm::lookAt(cameraPosn, cameraPosn + cameraFront, upVector);
    glm::mat4 Perspective = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, nearClippingVal, farClippingVal);

    glUniformMatrix4fv(glGetUniformLocation(shader, "View"), 1, GL_FALSE, glm::value_ptr(View));
    glUniformMatrix4fv(glGetUniformLocation(shader, "Perspective"), 1, GL_FALSE, glm::value_ptr(Perspective));

    glfwSetKeyCallback(window, keyCallBack);

    // Render Loop
    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwSetKeyCallback(window, keyCallBack);

        processWindowCloseInput(window);
        glUseProgram(shader);

        glm::mat4 View = glm::lookAt(cameraPosn, cameraPosn + cameraFront, upVector);
        glUniformMatrix4fv(glGetUniformLocation(shader, "View"), 1, GL_FALSE, glm::value_ptr(View));
        glUniformMatrix4fv(glGetUniformLocation(shader, "Perspective"), 1, GL_FALSE, glm::value_ptr(Perspective));

        // Drawing Models
        DrawModels(bodies);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
// For linux, run run_simulator.sh and for windows powershell, run run_simulator.ps1
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "../include/json.hpp"
#include <fstream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../include/Models.h"
#include "../include/SpaceTimeFabric.h"


// Configurations gathering from Configurations.json file
struct Config {
    float FoV;
    float nearClippingVal;
    float farClippingVal;
    glm::vec3 cameraPosn;
    glm::vec3 cameraFront;
    glm::vec3 upVector;
    float cameraSpeed;
    float mouseSensitivity;
    float gridStep;
    int gridSquares;
    float y_grid;
    float E_val_km;
    float E_val_kg;
    float distance_cutoff;
    float G_const;
    float min_dist;
    float deformation_scale;
    float time_step;
} configs;

// Global variables at start of program
GLFWmonitor* monitor = nullptr;
bool isFullscreen = false;
bool mouseInit = true;
int windowPosX, windowPosY;
int windowWidth = 1920;
int windowHeight = 1080;
double last_x, last_y;
float yaw = 0.0f;
float pitch = 0.0f;

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
    glm::mat4 Perspective = glm::perspective(glm::radians(configs.FoV), aspectRatio, configs.nearClippingVal, configs.farClippingVal);

    return;
}

void processWindowCloseInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    return;
}

void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {

    // Update cameraPosn View
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        configs.cameraPosn += configs.cameraSpeed * glm::normalize(glm::cross(configs.cameraFront, configs.upVector));
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        configs.cameraPosn -= configs.cameraSpeed * glm::normalize(glm::cross(configs.cameraFront, configs.upVector));
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        configs.cameraPosn += configs.cameraSpeed * configs.cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        configs.cameraPosn -= configs.cameraSpeed * configs.cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        configs.cameraPosn += configs.cameraSpeed * glm::normalize(glm::cross(glm::cross(configs.cameraFront, configs.upVector), configs.cameraFront));
    }
    if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) or (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)) {
        configs.cameraPosn -= configs.cameraSpeed * glm::normalize(glm::cross(glm::cross(configs.cameraFront, configs.upVector), configs.cameraFront));
    }

    return;
}

void mouseCallback(GLFWwindow* window, double mouse_x, double mouse_y) {
    // When program starts, capture x and y for the first time
    if (mouseInit) {
        last_x = mouse_x;
        last_y = mouse_y;
        mouseInit = false;
    }

    float delta_x = (mouse_x - last_x) * configs.mouseSensitivity;
    float delta_y = (last_y - mouse_y) * configs.mouseSensitivity;          // negative causes y-axis inversion

    last_x = mouse_x;
    last_y = mouse_y;

    // Changing angle of camera orientation depending on mouse distance change
    yaw += delta_x;
    pitch += delta_y;

    // Making sure camera stays limited to an up and down looking value
    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    float x_new = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
    float y_new = sinf(glm::radians(pitch));
    float z_new = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));

    // Updating camera orientation according to calculations
    configs.cameraFront = glm::normalize(glm::vec3(x_new, y_new, z_new));

    return;
}

void loadConfigs(const std::string filename) {
    std::ifstream inFile(filename);
    nlohmann::json json_file;
    inFile >> json_file;

    configs.FoV = json_file["FoV"];
    configs.nearClippingVal = json_file["nearClippingVal"];
    configs.farClippingVal = json_file["farClippingVal"];
    configs.cameraPosn = glm::vec3(json_file["cameraPosn"][0], json_file["cameraPosn"][1], json_file["cameraPosn"][2]);
    configs.cameraFront = glm::vec3(json_file["cameraFront"][0], json_file["cameraFront"][1], json_file["cameraFront"][2]);
    configs.upVector = glm::vec3(json_file["upVector"][0], json_file["upVector"][1], json_file["upVector"][2]);
    configs.cameraSpeed = json_file["cameraSpeed"];
    configs.mouseSensitivity = json_file["mouseSensitivity"];
    configs.gridStep = json_file["gridStep"];
    configs.gridSquares = json_file["gridSquares"];
    configs.y_grid = json_file["y_grid"];
    configs.E_val_km = json_file["E_val_km"];
    configs.E_val_kg = json_file["E_val_kg"];
    configs.distance_cutoff = json_file["distance_cutoff"];
    configs.G_const = json_file["G_const"];
    configs.G_const *= configs.E_val_kg / (glm::pow(configs.E_val_km, 3) * 1e9);
    configs.min_dist = json_file["min_dist"];
    configs.deformation_scale = json_file["deformation_scale"];
    configs.time_step = json_file["time_step"];

    return;
}

std::vector<Body> InitializeModels(GLuint shader) {
    
    Bodies bodies("data/BodiesData.json", configs.E_val_km, configs.E_val_kg, shader, configs.time_step);

    return bodies.get_bodies();
}

Fabric InitializeGrid(std::vector<Body> bodies_list, GLuint shader) {

    Fabric grid(bodies_list, configs.E_val_km, configs.E_val_kg, configs.distance_cutoff, configs.gridStep, configs.gridSquares, glm::vec3(0.0f, configs.y_grid, 0.0f), {1.0f, 1.0f, 1.0f, 1.0f}, configs.y_grid, configs.G_const, configs.min_dist, configs.deformation_scale, shader);

    return grid;
}

void DrawModels(std::vector<Body>& bodies) {
    int size = bodies.size();
    
    for (int idx = 0; idx < size; idx++) {
        bodies[idx].update_body(bodies, size, configs.G_const);
        bodies[idx].draw_body();
    }

    return;
}

void DrawGrid(Fabric grid, std::vector<Body>& bodies) {
    grid.draw_fabric(bodies);

    return;
}



// MAIN PROGRAM
int main() {
    // Loading Configurations
    loadConfigs("data/Configurations.json");

    // Starting OpenGL
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Creating window
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "3D Gravity Simulator", NULL, NULL);
    float aspectRatio = (float)windowWidth / (float)windowHeight;

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

    // Initializing Models and Space Time Fabric Grid
    // std::vector<Body> bodies = InitializeModels(shader);
    std::vector<Body> bodies = InitializeModels(shader);
    Fabric grid = InitializeGrid(bodies, shader);

    // Using shader program
    glUseProgram(shader);
    glEnable(GL_DEPTH_TEST);

    // Transformation matrices involved in 3D
    glm::mat4 View = glm::lookAt(configs.cameraPosn, configs.cameraPosn + configs.cameraFront, configs.upVector);
    glm::mat4 Perspective = glm::perspective(glm::radians(configs.FoV), aspectRatio, configs.nearClippingVal, configs.farClippingVal);

    glUniformMatrix4fv(glGetUniformLocation(shader, "View"), 1, GL_FALSE, glm::value_ptr(View));
    glUniformMatrix4fv(glGetUniformLocation(shader, "Perspective"), 1, GL_FALSE, glm::value_ptr(Perspective));

    glfwSetKeyCallback(window, keyCallBack);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);

    // Render Loop
    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwSetCursorPosCallback(window, mouseCallback);
        glfwSetKeyCallback(window, keyCallBack);

        processWindowCloseInput(window);
        glUseProgram(shader);

        glm::mat4 View = glm::lookAt(configs.cameraPosn, configs.cameraPosn + configs.cameraFront, configs.upVector);
        glUniformMatrix4fv(glGetUniformLocation(shader, "View"), 1, GL_FALSE, glm::value_ptr(View));
        glUniformMatrix4fv(glGetUniformLocation(shader, "Perspective"), 1, GL_FALSE, glm::value_ptr(Perspective));

        // Drawing Models
        DrawModels(bodies);
        DrawGrid(grid, bodies);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
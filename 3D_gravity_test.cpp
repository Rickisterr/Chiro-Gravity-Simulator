// For linux, run run_simulator.sh and for windows powershell, run run_simulator.ps1
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <glad/glad.h>
// #include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


// Default options values at starting
float FoV = 45;
float nearClippingVal = 0.5f;                                           // Too small causes precision issues
float farClippingVal = 100.0f;                                          // Too big requires too much computation
glm::vec3 cameraPosn = glm::vec3(0.0f, 0.0f, 0.0f);                     // camera position
// glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);                   // position of camera focus
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);                   // front direction of camera
// glm::vec3 cameraDirection = glm::normalize(cameraPosn - cameraTarget);  // direction of camera View
glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);                       // direction of camera's z axis
const float cameraSpeed = 0.5f;                                         // Speed of camera movements

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

// const char* vertexShaderScript = R"glsl(
//     #version 330 core
//     layout (location = 0) in vec3 Posn;
//     void main() {
//         gl_Position = vec4(Posn, 1.0);
//     }
// )glsl";

const char* fragmentShaderScript = R"glsl(
    #version 330 core
    out vec4 FragColor;
    uniform vec4 currentColor;
    void main() {
        FragColor = currentColor;
    }
)glsl";


class Body{
    /*
    Class for Models in simulation
    
    Args:
    VAO -> vertex array object referring to Model in memory
    VBO -> vertex buffer object describing Model's properties
    vertices -> list of vertices coordinates describing Model
    color -> vector of RGB values and opacity value in float
    shader -> shaders program script compiled, loaded, and linked into the graphics memory
    */
    public:
        GLuint VAO, VBO;
        glm::vec3 position;
        std::vector<float> vertices;
        GLuint shader;
        std::vector<float> color;

        Body(GLuint VAO, GLuint VBO, std::vector<float> vertices, glm::vec3 position, std::vector<float> color, GLuint shader) {
            this->VAO = VAO;
            this->VBO = VBO;

            this->position = position;
            this->vertices = vertices;
            this->color = color;

            this->shader = shader;

            create_body();

            return;
        }

        void create_body() {
            glGenVertexArrays(1, &this->VAO);
            glGenBuffers(1, &this->VBO);

            glBindVertexArray(this->VAO);
            glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
            glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindVertexArray(0);

            return;
        }

        void draw_body() {
            glm::mat4 Model = glm::mat4(1.0f);
            Model = glm::translate(Model, this->position);
            glUniformMatrix4fv(glGetUniformLocation(this->shader, "Model"), 1, GL_FALSE, glm::value_ptr(Model));

            glUniform4f(glGetUniformLocation(this->shader, "currentColor"), this->color[0], this->color[1], this->color[2], this->color[3]);
            glBindVertexArray(this->VAO);
            glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);

            return;
        }
};


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
        // printf("%f, %f, %f", cameraPosn.x, cameraPosn.y, cameraPosn.z);
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


// MAIN PROGRAM
int main() {

    // Starting OpenGL
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Creating window
    GLFWwindow* window = glfwCreateWindow(800, 600, "2D Gravity Simulator", NULL, NULL);

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
    // Planet 1
    GLuint VAO1, VBO1;
    std::vector<float> vertices_planet1 = {
        0.0f, 0.5f, 0.5f,
        0.5f, 0.0f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.0f, 0.0f, 0.5f,
        0.3f, 0.3f, 0.5f,
        0.6f, 0.6f, 0.5f
    };
    Body planet1(VAO1, VBO1, vertices_planet1, glm::vec3(0.0f, 0.0f, 0.5f), {1.0f, 0.0f, 0.0f, 1.0f}, shader);

    // Planet 2
    GLuint VAO2, VBO2;
    std::vector<float> vertices_planet2 = {
        -0.1f, -0.1f, 0.0f,
        0.1f, -0.1f, 0.0f,
        0.0f,  0.1f, 0.0f
    };
    Body planet2(VAO2, VBO2, vertices_planet2, glm::vec3(-0.2f, -0.2f, -0.5f), {0.0f, 0.0f, 1.0f, 1.0f}, shader);

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
        planet1.draw_body();
        planet2.draw_body();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
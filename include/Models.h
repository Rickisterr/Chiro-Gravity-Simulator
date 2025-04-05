#ifndef MODELS_H
#define MODELS_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern int rowsCount;
extern int columnsCount;
extern const float pi;

class Body {
    /*
    Class for Models in simulation
    
    Args:
    radius -> radius of body in OpenGL axis measurements
    position -> spatial position of center of sphere
    color -> vector of RGB values and opacity value in float
    shader -> shaders program script compiled, loaded, and linked into the graphics memory
    */
    public:
        GLuint VAO, VBO;
        float radius;
        glm::vec3 position;
        std::vector<float> vertices;
        GLuint shader;
        std::vector<float> color;

        Body(float radius, glm::vec3 position, std::vector<float> color, GLuint shader);
        void compute_vertices();
        void create_body();
        void draw_body();
};

#endif
#ifndef SPACETIMEFABRIC_H
#define SPACETIMEFABRIC_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Fabric {
    /*
    Class simulating the fabric of spacetime

    Args:
    position -> position of center of spacetime fabric
    gridStep -> distance between each row or column of the grid
    color -> color of the grid mesh simulating the fabric
    y_value -> y value of the static level of the grid
    shader -> shader program used for all models in the simulation
    */
    public:
        GLuint VAO, VBO;
        glm::vec3 position;
        float gridStep;
        int gridSquares;
        std::vector<float> vertices;
        std::vector<float> color;
        float y_value;
        GLuint shader;

        Fabric(float gridStep, int gridSquares, glm::vec3 position, std::vector<float> color, float y_value, GLuint shader);
        void compute_vertices();
        void create_fabric();
        void draw_fabric();
};

#endif
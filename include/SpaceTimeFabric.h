#ifndef SPACETIMEFABRIC_H
#define SPACETIMEFABRIC_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "../include/Models.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern float precision;

class Fabric {
    /*
    Class simulating the fabric of spacetime

    Args:
    bodies -> array of model bodies stored in json file
    position -> position of center of spacetime fabric
    gridStep -> distance between each row or column of the grid
    color -> color of the grid mesh simulating the fabric
    y_value -> y value of the static level of the grid
    shader -> shader program used for all models in the simulation
    */
    public:
        GLuint VAO, VBO;
        std::vector<Body> bodies;
        int E_val_km;
        int E_val_kg;
        float distance_cutoff;
        glm::vec3 position;
        float gridStep;
        int gridSquares;
        std::vector<float> vertices;
        std::vector<float> color;
        float y_value;
        float G_const;
        float min_dist;
        float deformation_scale;
        GLuint shader;

        Fabric(std::vector<Body> bodies, float E_val_km, float E_val_kg, float distance_cutoff, float gridStep, int gridSquares, glm::vec3 position, std::vector<float> color, float y_value, float G_const, float min_dist, float deformation_scale, GLuint shader);
        void compute_vertices();
        void create_fabric();
        void draw_fabric(std::vector<Body> bodies);
};

#endif
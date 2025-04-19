#ifndef MODELS_H
#define MODELS_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "../include/json.hpp"
#include <fstream>
#include <string>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern int rowsCount;
extern int columnsCount;
extern const float pi;
extern float precision;

class Body {
    /*
    Class for Models in simulation
    
    Args:
    diameter -> diameter of body in OpenGL axis measurements
    position -> spatial position of center of sphere
    color -> vector of RGB values and opacity value in float
    shader -> shaders program script compiled, loaded, and linked into the graphics memory
    */
    public:
        GLuint VAO, VBO;
        std::string name;
        float mass;
        float diameter;
        glm::vec3 position;
        std::vector<float> vertices;
        glm::vec3 velocity;
        std::vector<float> color;
        GLuint shader;
        float time_step;

        Body(std::string name, float mass, float diameter, glm::vec3 position, glm::vec3 init_velocity, std::vector<float> color, GLuint shader, float time_step);
        void compute_vertices();
        void create_body();
        void draw_body();
        void update_body(std::vector<Body> bodies, int bodies_num, float G_const);
};

class Bodies {
    public:
        std::vector<Body> bodies;
        float E_val_km;
        float E_val_kg;
    
        Bodies(const std::string filename, float E_val_km, float E_val_kg, GLuint shader, float time_step);
        int find_body_index(std::vector<std::string> body_names, std::string name);
        std::vector<Body> get_bodies();
};

#endif
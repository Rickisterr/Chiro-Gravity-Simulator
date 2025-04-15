#include "../include/Models.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "../include/json.hpp"
#include <fstream>
#include <string>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float pi = acosf(-1);

int rowsCount = 20;
int columnsCount = 30;

Body::Body(std::string name, float mass, float diameter, glm::vec3 position, std::vector<float> color, GLuint shader) {
    this->name = name;
    this->mass = mass;

    this->position = position;
    this->diameter = diameter;
    this->color = color;

    this->shader = shader;

    this->compute_vertices();
    this->create_body();

}

void Body::compute_vertices() {
    // Emptying vertices data
    std::vector<float>().swap(this->vertices);

    float rowStep = 2 * pi / rowsCount;
    float columnStep = pi / columnsCount;
    
    // For each quad in the sphere grid, 2 triangles
    for (int i = 0; i < columnsCount; i++) {
        
        for (int j = 0; j < rowsCount; j++) {

            // Vertex 1: (i, j)
            float columnAngle1 = (pi / 2) - (i * columnStep);
            float rowAngle1 = j * rowStep;
            float r_cosphi1 = this->diameter * cosf(columnAngle1);
            float x1 = r_cosphi1 * cosf(rowAngle1) + this->position.x;
            float y1 = r_cosphi1 * sinf(rowAngle1) + this->position.y;
            float z1 = this->diameter * sinf(columnAngle1) + this->position.z;
            
            // Vertex 2: (i+1, j)
            float columnAngle2 = (pi / 2) - ((i+1) * columnStep);
            float rowAngle2 = j * rowStep;
            float r_cosphi2 = this->diameter * cosf(columnAngle2);
            float x2 = r_cosphi2 * cosf(rowAngle2) + this->position.x;
            float y2 = r_cosphi2 * sinf(rowAngle2) + this->position.y;
            float z2 = this->diameter * sinf(columnAngle2) + this->position.z;
            
            // Vertex 3: (i, j+1)
            float columnAngle3 = (pi / 2) - (i * columnStep);
            float rowAngle3 = (j+1) * rowStep;
            float r_cosphi3 = this->diameter * cosf(columnAngle3);
            float x3 = r_cosphi3 * cosf(rowAngle3) + this->position.x;
            float y3 = r_cosphi3 * sinf(rowAngle3) + this->position.y;
            float z3 = this->diameter * sinf(columnAngle3) + this->position.z;
            
            // Vertex 4: (i+1, j+1)
            float columnAngle4 = (pi / 2) - ((i+1) * columnStep);
            float rowAngle4 = (j+1) * rowStep;
            float r_cosphi4 = this->diameter * cosf(columnAngle4);
            float x4 = r_cosphi4 * cosf(rowAngle4) + this->position.x;
            float y4 = r_cosphi4 * sinf(rowAngle4) + this->position.y;
            float z4 = this->diameter * sinf(columnAngle4) + this->position.z;
            
            // First triangle
            vertices.push_back(x1);
            vertices.push_back(y1);
            vertices.push_back(z1);
            
            vertices.push_back(x2);
            vertices.push_back(y2);
            vertices.push_back(z2);
            
            vertices.push_back(x3);
            vertices.push_back(y3);
            vertices.push_back(z3);
            
            // Second triangle
            vertices.push_back(x2);
            vertices.push_back(y2);
            vertices.push_back(z2);
            
            vertices.push_back(x4);
            vertices.push_back(y4);
            vertices.push_back(z4);
            
            vertices.push_back(x3);
            vertices.push_back(y3);
            vertices.push_back(z3);
        }
    }

    return;
}

void Body::draw_body() {
    glUniform4f(glGetUniformLocation(this->shader, "currentColor"), this->color[0], this->color[1], this->color[2], this->color[3]);
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);

    return;
}

void Body::create_body() {
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


Bodies::Bodies(const std::string filename, float E_val_km, float E_val_kg, GLuint shader) {
    // Power of 10 value of measurement in km and kg
    this->E_val_km = E_val_km;
    this->E_val_kg = E_val_kg;

    std::ifstream inFile(filename);
    nlohmann::json json_file;
    inFile >> json_file;

    float diameter;

    std::string name;
    float mass;
    std::vector<std::string> stars_name;
    std::vector<std::vector<float>> stars_posn;
    float init_distance_x;
    float init_distance_z;
    std::vector<float> color;
    std::string system;

    for (auto& star : json_file["stars"]) {
        name = star["name"];
        mass = float(star["mass (kg)"]) / this->E_val_kg;
        diameter = float(star["diameter (km)"]) / this->E_val_km;
        color = {star["color"][0], star["color"][1], star["color"][2], star["color"][3]};
        init_distance_x = float(star["center position (km)"][0]) / this->E_val_km;
        init_distance_z = float(star["center position (km)"][1]) / this->E_val_km;

        stars_posn.push_back({init_distance_x, diameter / 2, init_distance_z});
        stars_name.push_back(name);

        Body body(name, mass, diameter, glm::vec3(init_distance_x, diameter / 2, init_distance_z), color, shader);
        this->bodies.push_back(body);
    }

    for (auto& planet : json_file["planets"]) {
        name = planet["name"];
        mass = float(planet["mass (kg)"]) / this->E_val_kg;
        diameter = float(planet["diameter (km)"]) / this->E_val_km;
        init_distance_x = float(planet["init_distance (km)"]) / this->E_val_km;
        init_distance_z = 0;
        color = {planet["color"][0], planet["color"][1], planet["color"][2], planet["color"][3]};
        system = planet["system"];
        int temp_i = this->find_body_index(stars_name, system);

        if (temp_i != -1) {
            init_distance_x += stars_posn[temp_i][0];
            init_distance_z += stars_posn[temp_i][2];
        }
        
        Body body(name, mass, diameter, glm::vec3(init_distance_x, diameter / 2, init_distance_z), color, shader);
        this->bodies.push_back(body);
    }
}

int Bodies::find_body_index(std::vector<std::string> body_names, std::string name) {
    int size = body_names.size();

    for (int i = 0; i < size; i++) {

        if (body_names[i] == name) {
            return i;
        }
    }

    return -1;
}

std::vector<Body> Bodies::get_bodies() {
    return this->bodies;
}
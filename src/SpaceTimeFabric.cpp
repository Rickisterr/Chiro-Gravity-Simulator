#include "../include/SpaceTimeFabric.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "../include/Models.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


Fabric::Fabric(std::vector<Body> bodies, float E_val_km, float E_val_kg, float distance_cutoff, float gridStep, int gridSquares, glm::vec3 position, std::vector<float> color, float y_value, float G_const, float min_dist, float deformation_scale, GLuint shader) {
    this->bodies = bodies;
    this->E_val_km = E_val_km;
    this->E_val_kg = E_val_kg;

    this->distance_cutoff = distance_cutoff;

    this->gridStep = gridStep;
    this->position = position;

    if (gridSquares % 2 == 0) {
        this->gridSquares = gridSquares;
    }
    else {
        this->gridSquares = gridSquares + 1;
    }

    this->y_value = y_value;
    
    this->color = color;
    this->G_const = G_const;
    this->min_dist = min_dist;
    this->deformation_scale = deformation_scale;

    this->shader = shader;

    this->compute_vertices();
    this->create_fabric();

    return;
}

void Fabric::compute_vertices() {
    // Emptzing vertices vector before calculation
    // std::vector<float>().swap(this->vertices);
    vertices.clear();

    float x, z_1, z_2;
    float x_1, x_2, z;

    std::vector<float> masses;
    std::vector<glm::vec3> positions;

    int bodies_size = this->bodies.size();

    float distance;
    float gravity_field;

    // Getting masses and positions for all bodies in scene in order
    for (int body = 0; body < bodies_size; body++) {
        masses.push_back(bodies[body].mass);
        positions.push_back(bodies[body].position);
    }

    // Forming vertices for top-down lines of grid
    for (int idx = -this->gridSquares; idx <= this->gridSquares; idx++) {
        for (int idz = -this->gridSquares; idz < this->gridSquares; idz++) {

            // Calculating first coordinate for a line segment
            x = idx * this->gridStep;
            z_1 = idz * this->gridStep;

            // Calculating second coordinate for a line segment
            z_2 = (idz + 1) * this->gridStep;

            gravity_field = 0;

            for (int body = 0; body < bodies_size; body++) {
                distance = glm::sqrt(glm::pow(x - positions[body][0], 2) + glm::pow(z_1 - positions[body][2], 2) + glm::pow(0 - positions[body][1], 2));

                if (distance <= this->min_dist) {
                    distance = this->min_dist;
                }

                if (distance <= distance_cutoff) {
                    gravity_field += ((this->G_const * masses[body]) / (distance * distance));
                }
            }

            this->vertices.push_back(x);
            this->vertices.push_back(this->y_value - gravity_field * this->deformation_scale);
            this->vertices.push_back(z_1);

            gravity_field = 0;

            for (int body = 0; body < bodies_size; body++) {
                distance = glm::sqrt(glm::pow(x - positions[body][0], 2) + glm::pow(z_2 - positions[body][2], 2) + glm::pow(0 - positions[body][1], 2));

                if (distance <= this->min_dist) {
                    distance = this->min_dist;
                }

                if (distance <= distance_cutoff) {
                    gravity_field += ((this->G_const * masses[body]) / (distance * distance));
                }
            }

            this->vertices.push_back(x);
            this->vertices.push_back(this->y_value - gravity_field * this->deformation_scale);
            this->vertices.push_back(z_2);
        }
    }

    // Forming vertices for left-right lines of grid
    for (int idz = -this->gridSquares; idz <= this->gridSquares; idz++) {
        for (int idx = -this->gridSquares; idx < this->gridSquares; idx++) {

            // Calculating first coordinate for a line segment
            x_1 = idx * this->gridStep;
            z = idz * this->gridStep;

            // Calculating second coordinate for a line segment
            x_2 = (idx + 1) * this->gridStep;

            gravity_field = 0;

            for (int body = 0; body < bodies_size; body++) {
                distance = glm::sqrt(glm::pow(x_1 - positions[body][0], 2) + glm::pow(z - positions[body][2], 2) + glm::pow(0 - positions[body][1], 2));

                if (distance <= this->min_dist) {
                    distance = this->min_dist;
                }

                if (distance <= distance_cutoff) {
                    gravity_field += ((this->G_const * masses[body]) / (distance * distance));
                }
            }

            this->vertices.push_back(x_1);
            this->vertices.push_back(this->y_value - gravity_field * this->deformation_scale);
            this->vertices.push_back(z);

            gravity_field = 0;

            for (int body = 0; body < bodies_size; body++) {
                distance = glm::sqrt(glm::pow(x_2 - positions[body][0], 2) + glm::pow(z - positions[body][2], 2) + glm::pow(0 - positions[body][1], 2));

                if (distance <= this->min_dist) {
                    distance = this->min_dist;
                }
                
                if (distance <= distance_cutoff) {
                    gravity_field += ((this->G_const * masses[body]) / (distance * distance));
                }
            }

            this->vertices.push_back(x_2);
            this->vertices.push_back(this->y_value - gravity_field * this->deformation_scale);
            this->vertices.push_back(z);
        }
    }

    return;
}

void Fabric::draw_fabric(std::vector<Body> bodies) {
    this->bodies = bodies;
    
    this->compute_vertices();
    this->create_fabric();

    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::translate(Model, this->position);
    glUniformMatrix4fv(glGetUniformLocation(this->shader, "Model"), 1, GL_FALSE, glm::value_ptr(Model));

    glUniform4f(glGetUniformLocation(this->shader, "currentColor"), this->color[0], this->color[1], this->color[2], this->color[3]);
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_LINES, 0, vertices.size() / 3);

    return;
}

void Fabric::create_fabric() {
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
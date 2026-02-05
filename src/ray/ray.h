#pragma once

#include <glad/glad.h>

typedef struct {
    double x_pos;
    double y_pos;
    double x_ang;
    double y_ang;

    unsigned int VBO; // Vertex Buffer Object holds all raw vertex data for GPU
    unsigned int VAO; // Vertex Array Object describes structure of VBO for GPU
} Ray;

int ray_init(Ray* ray, double x_pos, double y_pos, double x_ang, double y_ang); 
void ray_step(Ray* ray, double dt_seconds, double time_scale);
void ray_draw(Ray* ray);
void ray_destroy(Ray* ray);

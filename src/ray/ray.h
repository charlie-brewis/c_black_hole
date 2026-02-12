#pragma once

#include <glad/glad.h>
#include <stddef.h>
#include "blackhole/blackhole.h"

typedef struct {
    // -- Cartesian Coordinates --
    double x_pos;
    double y_pos;
    double x_ang;
    double y_ang;
    
    // -- Polar coordinates --
    // r: distance from black hole
    // phi: angle from the x-axis
    double r; double phi;

    unsigned int VBO; // Vertex Buffer Object holds all raw vertex data for GPU
    unsigned int VAO; // Vertex Array Object describes structure of VBO for GPU

    size_t trail_capacity;
    size_t trail_count;
    size_t trail_head;
    float* trail_positions; // x,y pairs in meters
    float* trail_staging;   // x,y,alpha interleaved for GPU
} Ray;

int ray_init(Ray* ray, double x_pos, double y_pos, double x_ang, double y_ang); 
void ray_step(Ray* ray, BlackHole* bh,  double dt_seconds, double time_scale);
void ray_draw(Ray* ray);
void ray_destroy(Ray* ray);

#pragma once

#include <glad/glad.h>
#include <stddef.h>
#include "blackhole/blackhole.h"

typedef struct {
    // -- Cartesian Coordinates --
    double x_pos;
    double y_pos;
    
    // -- Polar coordinates --
    // r: distance from black hole
    // phi: angle from the x-axis
    double r; double phi;
    // Radial/angular derivatives are stored to retain the ray's direction of travel
    // between integration steps (so the geodesic evolves smoothly).
    double dr; double dphi;
    
    // To solve null geodestics in Schwarzchild spacetime, two quantities are conserved.
    // 1) Angular momentum per unit mass (L) - because spacetime is rotationally symmetric. This depends on the position and angular velocity of the ray.
    // 2) Energy per unit mass (E) - this depends on the blackhole via r_s. This means we need knowledge of the blackhole to initialise the ray.
    double E; double L; 

    unsigned int VBO; // Vertex Buffer Object holds all raw vertex data for GPU
    unsigned int VAO; // Vertex Array Object describes structure of VBO for GPU

    size_t trail_capacity;
    size_t trail_count;
    size_t trail_head;
    float* trail_positions; // x,y pairs in meters
    float* trail_staging;   // x,y,alpha interleaved for GPU
} Ray;

int ray_init(Ray* ray, BlackHole* bh,  double x_pos, double y_pos); 
void ray_step(Ray* ray, BlackHole* bh,  double dt_seconds, double time_scale);
void ray_draw(Ray* ray);
void ray_destroy(Ray* ray);

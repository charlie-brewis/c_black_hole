#pragma once

#include <glad/glad.h>

#define _NUM_SEGMENTS 100

// Obviously we don't want to render the black hole(s) in real scale, so we need to normalise
// We are using the mass of SagA as the reference and scaling any other black holes relative to it
#define REFERENCE_MASS 8.54e36
// This is the size SagA will appear on screen
#define TARGET_NDC_RADIUS 0.15

#define G 6.67430e-11
#define C 299792458.0

typedef struct {
    double x, y;
    double mass;
    double schwarz_r;

    unsigned int VBO; // Vertex Buffer Object holds all raw vertex data for GPU
    unsigned int VAO; // Vertex Array Object describes structure of VBO for GPU
} BlackHole;

int blackhole_init(BlackHole* bh, double x, double y, double mass);
void blackhole_draw(BlackHole* bh);
void blackhole_destroy(BlackHole* bh);

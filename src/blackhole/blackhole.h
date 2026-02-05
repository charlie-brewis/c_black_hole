#pragma once

#include <glad/glad.h>

#define _NUM_SEGMENTS 100

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

#include "blackhole.h"
#include <math.h>
#include <stddef.h>

static double schwarzschild_radius(double mass) {
    return (2 * G * mass) / (C * C);
}

int blackhole_init(BlackHole* bh, double x, double y, double mass) {
    if (!bh) return -1; 

    bh->x = x;
    bh->y = y;
    bh->mass = mass;

    bh->schwarz_r = schwarzschild_radius(mass);

    // Generate VB0 + VAO
    glGenBuffers(1, &bh->VBO);
    glGenVertexArrays(1, &bh->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, bh->VBO);
    glBindVertexArray(bh->VAO);

    // Allocate the buffer once (data will be filled on draw)
    glBufferData(
        GL_ARRAY_BUFFER,
        (_NUM_SEGMENTS + 2) * 2 * sizeof(float), // See `blackhole_draw` for size reasoning
        NULL,
        GL_DYNAMIC_DRAW
    );
    
    // Defines how the GPU reads VBO. start=0; num_components=2; data_type=float; normalise=false; stride=2*sizeof(float); offset=0;
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  // This just turns on attribute slot 0

    glBindVertexArray(0);

    return 0;
}

void blackhole_draw(BlackHole *bh) {
    const double render_scale = TARGET_NDC_RADIUS / schwarzschild_radius(REFERENCE_MASS);
    const float draw_r = (float)(bh->schwarz_r * render_scale);
    const float cx = (float)(bh->x * render_scale);
    const float cy = (float)(bh->y * render_scale);

    // Define the vertex buffer
    // Each segment needs a vertex (+ center + closing vertex (since it's a triangle fan)), and each vertex needs 2 data points (x, y).
    float vertices[(_NUM_SEGMENTS + 2) * 2];

    // Populate vertex buffer
    vertices[0] = cx;
    vertices[1] = cy;
    for (int i = 0; i <=_NUM_SEGMENTS; i++) {
        float angle = 2.0f * M_PI * i / _NUM_SEGMENTS;

        //! Not sure about the indexing here? For example vertices[6, 7] aren't used?
        vertices[2 + i*2] = cosf(angle) * draw_r + cx;
        vertices[3 + i*2] = sinf(angle) * draw_r + cy;
    }
    
    // Have to rebind VAO on each draw in case OpenGL has had a different VAO bound
    glBindVertexArray(bh->VAO);

    // Have to rebind the VBO because we are updating it's data - `glBufferSubData` always updates the currently bound VBO
    glBindBuffer(GL_ARRAY_BUFFER, bh->VBO);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        sizeof(vertices),
        vertices
    );
    
    // Call to actually draw the vertices
    glDrawArrays(GL_TRIANGLE_FAN, 0, _NUM_SEGMENTS + 2);
    // Unbind VAO to avoid accidental misuse from other draw calls
    glBindVertexArray(0);
}

void blackhole_destroy(BlackHole* bh) {
    if (!bh) return;

    if (bh->VBO) {
        glDeleteBuffers(1, &bh->VBO);
        bh->VBO = 0;
    }

    if (bh->VAO) {
        glDeleteVertexArrays(1, &bh->VAO);
        bh->VAO = 0;
    }
}

#include "blackhole.h"
#include "engine/gl_utils.h"
#include "render/render_scale.h"
#include <math.h>
#include <stddef.h>

int blackhole_init(BlackHole* bh, double x, double y, double mass) {
    if (!bh) return -1; 

    bh->x = x;
    bh->y = y;
    bh->mass = mass;

    bh->schwarz_r = schwarzschild_radius(mass);

    if (gl_setup_vao_vbo(
        &bh->VAO,
        &bh->VBO,
        (_NUM_SEGMENTS + 2) * 2 * sizeof(float),
        2,
        GL_DYNAMIC_DRAW
    ) != 0) {
        return -1;
    }

    return 0;
}

void blackhole_draw(BlackHole *bh) {
    const double scale = render_scale();
    const float draw_r = (float)(bh->schwarz_r * scale);
    const float cx = (float)(bh->x * scale);
    const float cy = (float)(bh->y * scale);

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
    gl_destroy_vao_vbo(&bh->VAO, &bh->VBO);
}

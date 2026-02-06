#pragma once

#include <glad/glad.h>
#include <stddef.h>

int gl_setup_vao_vbo(
    unsigned int* vao,
    unsigned int* vbo,
    size_t buffer_bytes,
    int components,
    GLenum usage,
    size_t stride_bytes,
    int extra_components,
    size_t extra_offset_bytes
);

void gl_destroy_vao_vbo(unsigned int* vao, unsigned int* vbo);

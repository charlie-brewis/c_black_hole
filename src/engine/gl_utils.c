#include "gl_utils.h"

int gl_setup_vao_vbo(
    unsigned int* vao,
    unsigned int* vbo,
    size_t buffer_bytes,
    int components,
    GLenum usage,
    size_t stride_bytes,
    int extra_components,
    size_t extra_offset_bytes
) {
    if (!vao || !vbo || components <= 0) return -1;

    if (stride_bytes == 0) {
        // Default to tightly packed attribute data.
        stride_bytes = components * sizeof(float);
    }

    // Generate VBO + VAO
    glGenVertexArrays(1, vao);
    glGenBuffers(1, vbo);
    
    // Bind VBO + VAO
    glBindVertexArray(*vao);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);

    // Allocate buffer space (data will be filled on draw)
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)buffer_bytes, NULL, usage);
    // Define how the GPU reads VBO in VAO. (start_i, num_components, data_type, normalise, stride, offset)
    // Attribute 0: positions.
    glVertexAttribPointer(0, components, GL_FLOAT, GL_FALSE, (GLsizei)stride_bytes, (void*)0);
    glEnableVertexAttribArray(0);
    if (extra_components > 0) {
        // Attribute 1: optional per-vertex data (e.g., alpha).
        glVertexAttribPointer(
            1,
            extra_components,
            GL_FLOAT,
            GL_FALSE,
            (GLsizei)stride_bytes,
            (void*)extra_offset_bytes
        );
        glEnableVertexAttribArray(1);
    } else {
        // Default alpha attribute to 1.0 for VAOs that don't supply it.
        glDisableVertexAttribArray(1);
        glVertexAttrib1f(1, 1.0f);
    }
    
    // Unbind so they aren't assigned for draw upon creation
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return 0;
}

void gl_destroy_vao_vbo(unsigned int* vao, unsigned int* vbo) {
    if (vbo && *vbo) {
        glDeleteBuffers(1, vbo);
        *vbo = 0;
    }
    if (vao && *vao) {
        glDeleteVertexArrays(1, vao);
        *vao = 0;
    }
}

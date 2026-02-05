#include "gl_utils.h"

int gl_setup_vao_vbo(
    unsigned int* vao,
    unsigned int* vbo,
    size_t buffer_bytes,
    int components,
    GLenum usage
) {
    if (!vao || !vbo || components <= 0) return -1;

    // Generate VBO + VAO
    glGenVertexArrays(1, vao);
    glGenBuffers(1, vbo);
    
    // Bind VBO + VAO
    glBindVertexArray(*vao);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);

    // Allocate buffer space (data will be filled on draw)
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)buffer_bytes, NULL, usage);
    // Define how the GPU reads VBO in VAO. (start_i, num_components, data_type, normalise, stride, offset)
    glVertexAttribPointer(0, components, GL_FLOAT, GL_FALSE, components * (GLsizei)sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
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

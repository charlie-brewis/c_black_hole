#include "engine.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

int engine_init(Engine* engine, int width, int height, char* name) {
    engine->width = width;
    engine->height = height;

    if (!glfwInit()) {
        fprintf(stderr, "Failed to init GLFW\n");
        return -1;
    }

    // OpenGL version 3.3 core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window
    engine->window = glfwCreateWindow(
            engine->width,
            engine->height,
            name,
            NULL,
            NULL
    );

    if (!engine->window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(engine->window);
    return 0;
}

void engine_run(Engine *engine) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void engine_destroy(Engine *engine) {
    glfwDestroyWindow(engine->window);
    glfwTerminate();
}

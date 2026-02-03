#pragma once
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct {
    GLFWwindow* window;
    int width;
    int height;
} Engine;

int engine_init(Engine* engine, int width, int height, char* name);
void engine_run(Engine* engine);
void engine_destroy(Engine* engine);

#pragma once
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct {
    GLFWwindow* window;
    unsigned int shaderProgram;
    int u_aspect_loc;
    int fb_width;
    int fb_height;
    float aspect;
    int width;
    int height;
} Engine;

int engine_init(Engine* engine, int width, int height, char* name);
void engine_begin_frame(Engine* engine);
void engine_end_frame(Engine* engine);
void engine_destroy(Engine* engine);

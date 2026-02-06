#include "engine.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

// Helper to compile the shader program
static unsigned int compile_shader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Shader compilation failed: %s\n", infoLog);
        exit(-1);
    }

    return shader;
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    Engine* eng = (Engine*)glfwGetWindowUserPointer(window);
    if (!eng) return;
    eng->fb_width = width;
    eng->fb_height = height;
    eng->aspect = (width == 0) ? 1.0f : (float)height / (float)width;
}

static void compile_shader_program(Engine* engine) {
    // Define vertex and fragment shader programs to send to GPU
    const char* vertexShaderSource = \
        "#version 330 core\n"
        "layout(location = 0) in vec2 aPos;\n"
        "layout(location = 1) in float aAlpha;\n"
        "uniform float u_aspect;\n"
        "out float v_alpha;\n"
        "void main() {\n"
        "  vec2 pos = aPos;\n"
        "  pos.x *= u_aspect;\n"
        "  gl_Position = vec4(pos, 0.0, 1.0);\n"
        "  v_alpha = aAlpha;\n"
        "}\0";
    const char* fragmentShaderSource = \
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "uniform vec4 u_color;\n"
        "in float v_alpha;\n"
        "void main() {\n"
        "   vec3 rgb = u_color.rgb * v_alpha;\n"
        "   float a = u_color.a * v_alpha;\n"
        "   FragColor = vec4(rgb, a);\n"
        "}\0";
        
    unsigned int vertexShader = compile_shader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compile_shader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    engine->shaderProgram = glCreateProgram();
    glAttachShader(engine->shaderProgram, vertexShader);
    glAttachShader(engine->shaderProgram, fragmentShader);
    glLinkProgram(engine->shaderProgram);

    // Check linking
    int success;
    char infoLog[512];
    glGetProgramiv(engine->shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(engine->shaderProgram, 512, NULL, infoLog);
    fprintf(stderr, "Shader program linking failed: %s\n", infoLog);
        exit(-1);
    }

    // Shaders can be deleted after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    engine->u_aspect_loc = glGetUniformLocation(engine->shaderProgram, "u_aspect");
    engine->u_color_loc = glGetUniformLocation(engine->shaderProgram, "u_color");
}

int engine_init(Engine* engine, int width, int height, char* name) {
    engine->width = width;
    engine->height = height;
    engine->fb_width = width;
    engine->fb_height = height;
    engine->aspect = 1.0f;
    engine->time_scale = 1.0;

    if (!glfwInit()) {
        fprintf(stderr, "Failed to init GLFW\n");
        return -1;
    }

    // OpenGL version 3.3 core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window
    engine->window = glfwCreateWindow(engine->width, engine->height, name, NULL, NULL);

    if (!engine->window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(engine->window);

    // Load OpenGL functions
     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
         fprintf(stderr, "Failed to initialise GLAD\n");
         return -1;
     }

    // Set up FramBuffer so that we can maintain proportions with changing aspect ratios
    glfwSetWindowUserPointer(engine->window, engine);
    glfwSetFramebufferSizeCallback(engine->window, framebuffer_size_callback);

    int fbw = 0;
    int fbh = 0;
    glfwGetFramebufferSize(engine->window, &fbw, &fbh);
    glViewport(0, 0, fbw, fbh);
    engine->fb_width = fbw;
    engine->fb_height = fbh;
    engine->aspect = (fbw == 0) ? 1.0f : (float)fbh / (float)fbw;
    
    // Compile the shaders
    compile_shader_program(engine); 
    return 0;
}

void engine_begin_frame(Engine *engine) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (engine->shaderProgram && engine->u_aspect_loc >= 0) {
        glUseProgram(engine->shaderProgram);
        glUniform1f(engine->u_aspect_loc, engine->aspect);
    }
}

void engine_end_frame(Engine *engine) {
    glfwSwapBuffers(engine->window);
    glfwPollEvents();
}

void engine_destroy(Engine *engine) {
    glfwDestroyWindow(engine->window);
    glfwTerminate();
}

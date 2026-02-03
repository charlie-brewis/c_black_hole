#include "engine/engine.h"

int main() {
    Engine engine;

    if (engine_init(&engine, 800, 600, "Black Hole Simulation") != 0) return -1;

    // Load OpenGL functions
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // main loop
    while (!glfwWindowShouldClose(engine.window)) {
        engine_run(&engine);
        glfwSwapBuffers(engine.window);
        glfwPollEvents();
    }

    engine_destroy(&engine);
    return 0;
}

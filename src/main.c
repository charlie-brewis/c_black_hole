#include "engine/engine.h"
#include "blackhole/blackhole.h"

int main() {
    Engine engine;
    if (engine_init(&engine, 800, 600, "Black Hole Simulation") != 0) return -1;
    
    BlackHole sagA;
    blackhole_init(&sagA, 0.0, 0.0, 8.54e36);

    // main loop
    while (!glfwWindowShouldClose(engine.window)) {
        engine_begin_frame(&engine);
        
        glUseProgram(engine.shaderProgram);

        blackhole_draw(&sagA);

        engine_end_frame(&engine);
    }

    blackhole_destroy(&sagA);
    engine_destroy(&engine);
    return 0;
}

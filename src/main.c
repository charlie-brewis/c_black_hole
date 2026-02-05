#include "engine/engine.h"
#include "blackhole/blackhole.h"
#include "ray/ray.h"
#include "render/render_scale.h"
#include <GLFW/glfw3.h>

int main() {
    Engine engine;
    if (engine_init(&engine, 800, 600, "Black Hole Simulation") != 0) return -1;
    
    BlackHole sagA;
    blackhole_init(&sagA, 0.0, 0.0, 8.54e36);

    Ray ray;
    ray_init(&ray, ndc_to_meters(-1), 0.0, 1.0, 0.0);

    // main loop
    engine.time_scale = 80.0;  // time_scale to 80 as sagA has a diameter of ~80 light seconds
    double last_time = glfwGetTime();
    while (!glfwWindowShouldClose(engine.window)) {
        const double now = glfwGetTime();
        const double dt = now - last_time;
        last_time = now;

        engine_begin_frame(&engine);
        
        glUseProgram(engine.shaderProgram);

        if (engine.u_color_loc >= 0) {
            glUniform4f(engine.u_color_loc, 1.0f, 0.0f, 0.0f, 1.0f);
        }
        blackhole_draw(&sagA);

        ray_step(&ray, dt, engine.time_scale);
        if (engine.u_color_loc >= 0) {
            glUniform4f(engine.u_color_loc, 1.0f, 1.0f, 1.0f, 1.0f);
        }
        ray_draw(&ray);

        engine_end_frame(&engine);
    }

    ray_destroy(&ray);
    blackhole_destroy(&sagA);
    engine_destroy(&engine);
    return 0;
}

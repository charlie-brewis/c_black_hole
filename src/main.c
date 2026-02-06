#include "engine/engine.h"
#include "blackhole/blackhole.h"
#include "ray/ray.h"
#include "render/render_scale.h"
#include <GLFW/glfw3.h>

#include <stdlib.h>

#define NUM_RAYS 7

int main() {
    Engine engine;
    if (engine_init(&engine, 800, 600, "Black Hole Simulation") != 0) return -1;
    
    BlackHole sagA;
    blackhole_init(&sagA, 0.0, 0.0, 8.54e36);

    Ray* rays = (Ray*) calloc(NUM_RAYS, sizeof(Ray));
    float spawn_margin = 0.05f; // 5% padding
    float span = 2.0f - 2.0f * spawn_margin;
    float y_step = (NUM_RAYS > 1) ? (span / (NUM_RAYS - 1)) : 0.0f;
    for (int i = 0; i < NUM_RAYS; i++) {
        float y_ndc = -1.0f + spawn_margin + y_step * i;  // Evenly spaced, centered around y=0
        ray_init(
            &rays[i], 
            ndc_to_meters(-1.0f / engine.aspect),
            ndc_to_meters(y_ndc),
            1.0,
            0.0
        );
    }

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

        if (engine.u_color_loc >= 0) {
            glUniform4f(engine.u_color_loc, 1.0f, 1.0f, 1.0f, 1.0f);
        }
        for (int i = 0; i < NUM_RAYS; i++) {
            ray_step(&rays[i], dt, engine.time_scale);
            ray_draw(&rays[i]);
        }

        engine_end_frame(&engine);
    }
    
    for (int i = 0; i < NUM_RAYS; i++) {
        ray_destroy(&rays[i]);
    }
    free(rays);
    rays = NULL;
    blackhole_destroy(&sagA);
    engine_destroy(&engine);
    return 0;
}

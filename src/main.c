#include "engine/engine.h"
#include "blackhole/blackhole.h"
#include "ray/ray.h"
#include "render/render_scale.h"
#include <GLFW/glfw3.h>

#include <math.h>
#include <stdlib.h>

#define DEFAULT_NUM_RAYS 10

int main(int argc, char** argv) {
    int num_rays = DEFAULT_NUM_RAYS;
    if (argc > 1) {
        const int parsed = atoi(argv[1]);
        if (parsed > 0) {
            num_rays = parsed;
        }
    }
    Engine engine;
    if (engine_init(&engine, 800, 600, "Black Hole Simulation") != 0) return -1;
    
    BlackHole sagA;
    blackhole_init(&sagA, 0.0, 0.0, 8.54e36);

    Ray* rays = (Ray*) calloc(num_rays, sizeof(Ray));
    const double angle_step = (num_rays > 0) ? (2.0 * M_PI / (double)num_rays) : 0.0;
    for (int i = 0; i < num_rays; i++) {
        const double angle = angle_step * (double)i;
        ray_init(
            &rays[i], 
            &sagA,
            sagA.schwarz_r * 1.5,
            ndc_to_meters(0.0f),
            angle
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
        for (int i = 0; i < num_rays; i++) {
            ray_step(&rays[i], &sagA, dt, engine.time_scale);
            ray_draw(&rays[i]);
        }

        engine_end_frame(&engine);
    }
    
    for (int i = 0; i < num_rays; i++) {
        ray_destroy(&rays[i]);
    }
    free(rays);
    rays = NULL;
    blackhole_destroy(&sagA);
    engine_destroy(&engine);
    return 0;
}

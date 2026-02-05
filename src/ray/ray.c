#include "ray.h"
#include "engine/gl_utils.h"
#include "engine/engine.h"
#include "render/render_scale.h"
#include <math.h>

int ray_init(Ray *ray, double x_pos, double y_pos, double x_ang, double y_ang) {
    if (!ray) return -1;
    
    ray->x_pos = x_pos;
    ray->y_pos = y_pos;
    ray->x_ang = x_ang;
    ray->y_ang = y_ang;
    const double mag = sqrt((x_ang * x_ang) + (y_ang * y_ang));
    if (mag > 0.0) {
        ray->x_ang /= mag;
        ray->y_ang /= mag;
    }

    if (gl_setup_vao_vbo(
        &ray->VAO,
        &ray->VBO,
        2 * sizeof(float),
        2,
        GL_DYNAMIC_DRAW
    ) != 0) {
        return -1;
    }

    return 0;
}

void ray_step(Ray* ray, double dt_seconds, double time_scale) {
    if (!ray) return;

    const double scaled_dt = dt_seconds * time_scale; 
    ray->x_pos += ray->x_ang * C * scaled_dt;
    ray->y_pos += ray->y_ang * C * scaled_dt;
}

void ray_draw(Ray *ray) {
    if (!ray) return;

    const double scale = render_scale();
    const float vx = (float)(ray->x_pos * scale);
    const float vy = (float)(ray->y_pos * scale);
    const float vertex[2] = { vx, vy };

    glBindVertexArray(ray->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, ray->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex), vertex);

    glPointSize(4.0f);
    glDrawArrays(GL_POINTS, 0, 1);

    glBindVertexArray(0);
}

void ray_destroy(Ray *ray) {
    if (!ray) return;
    gl_destroy_vao_vbo(&ray->VAO, &ray->VBO);
}

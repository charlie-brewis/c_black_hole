#include "ray.h"
#include "engine/gl_utils.h"
#include "render/render_scale.h"
#include <math.h>
#include <stdlib.h>

static const size_t RAY_TRAIL_CAPACITY = 2048;
static const float RAY_TRAIL_MIN_ALPHA = 0.2f;

static void ray_trail_push(Ray* ray, float x, float y) {
    if (!ray || !ray->trail_positions || ray->trail_capacity == 0) return;

    // Ring buffer: append until full, then overwrite the oldest entry.
    size_t idx = (ray->trail_head + ray->trail_count) % ray->trail_capacity;
    ray->trail_positions[2 * idx] = x;
    ray->trail_positions[2 * idx + 1] = y;

    if (ray->trail_count < ray->trail_capacity) {
        ray->trail_count++;
    } else {
        ray->trail_head = (ray->trail_head + 1) % ray->trail_capacity;
    }
}

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

    ray->trail_capacity = RAY_TRAIL_CAPACITY;
    ray->trail_count = 0;
    ray->trail_head = 0;
    ray->trail_positions = (float*)calloc(ray->trail_capacity * 2, sizeof(float));
    ray->trail_staging = (float*)calloc(ray->trail_capacity * 3, sizeof(float));
    if (!ray->trail_positions || !ray->trail_staging) {
        free(ray->trail_positions);
        free(ray->trail_staging);
        ray->trail_positions = NULL;
        ray->trail_staging = NULL;
        return -1;
    }

    // Interleaved layout: [x, y, alpha] per vertex.
    if (gl_setup_vao_vbo(
        &ray->VAO,
        &ray->VBO,
        (size_t)(ray->trail_capacity * 3 * sizeof(float)),
        2,
        GL_DYNAMIC_DRAW,
        3 * sizeof(float),
        1,
        2 * sizeof(float)
    ) != 0) {
        free(ray->trail_positions);
        free(ray->trail_staging);
        ray->trail_positions = NULL;
        ray->trail_staging = NULL;
        return -1;
    }

    ray_trail_push(ray, (float)ray->x_pos, (float)ray->y_pos);
    return 0;
}

void ray_step(Ray* ray, double dt_seconds, double time_scale) {
    if (!ray) return;

    const double scaled_dt = dt_seconds * time_scale; 
    ray->x_pos += ray->x_ang * C * scaled_dt;
    ray->y_pos += ray->y_ang * C * scaled_dt;
    ray_trail_push(ray, (float)ray->x_pos, (float)ray->y_pos);
}

void ray_draw(Ray *ray) {
    if (!ray) return;
    if (!ray->trail_staging || ray->trail_count == 0) return;

    const double scale = render_scale();
    const size_t count = ray->trail_count;
    // Older points get smaller alpha so the trail fades behind the ray.
    for (size_t i = 0; i < count; i++) {
        const size_t idx = (ray->trail_head + i) % ray->trail_capacity;
        const float x = ray->trail_positions[2 * idx];
        const float y = ray->trail_positions[2 * idx + 1];
        const float t = (count <= 1) ? 1.0f : (float)i / (float)(count - 1);
        const float alpha = RAY_TRAIL_MIN_ALPHA + (1.0f - RAY_TRAIL_MIN_ALPHA) * t;

        ray->trail_staging[3 * i] = (float)(x * scale);
        ray->trail_staging[3 * i + 1] = (float)(y * scale);
        ray->trail_staging[3 * i + 2] = alpha;
    }

    glBindVertexArray(ray->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, ray->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(count * 3 * sizeof(float)), ray->trail_staging);

    glLineWidth(2.0f);
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)count);
    glPointSize(4.0f);
    glDrawArrays(GL_POINTS, (GLint)(count - 1), 1);

    glBindVertexArray(0);
}

void ray_destroy(Ray *ray) {
    if (!ray) return;
    free(ray->trail_positions);
    free(ray->trail_staging);
    ray->trail_positions = NULL;
    ray->trail_staging = NULL;
    ray->trail_capacity = 0;
    ray->trail_count = 0;
    ray->trail_head = 0;
    gl_destroy_vao_vbo(&ray->VAO, &ray->VBO);
}

#include "ray.h"
#include "blackhole/blackhole.h"
#include "engine/gl_utils.h"
#include "physics/constants.h"
#include "render/render_scale.h"
#include <math.h>
#include <stdlib.h>

static const size_t RAY_TRAIL_CAPACITY = 2048;
static const float RAY_TRAIL_MIN_ALPHA = 0.2f;

static void ray_init_derivs(Ray* ray, double init_angle) {
    // Initial direction in world-space (radians from +X).
    const double delta = init_angle - ray->phi;
    ray->dr = C * cos(delta);
    ray->dphi = (ray->r == 0.0) ? 0.0 : (C * sin(delta) / ray->r);
}

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

int ray_init(Ray *ray, BlackHole* bh,  double x, double y, double init_angle) {
    if (!ray) return -1;
    
    ray->x = x;
    ray->y = y;

    // Position relative to the black hole center.
    const double dx = ray->x - bh->x;
    const double dy = ray->y - bh->y;
    ray->r = sqrt(dx * dx + dy * dy);
    ray->phi = atan2(dy, dx);
    ray_init_derivs(ray, init_angle);

    // Define physical concervation values. See equations:
    // L = r^2 * (dphi / dlambd)
    // E = f * (dt / dlambd)
    //  where
    //      f = 1 - r_s / r
    //
    // The null condition must hold for a null geodesic:
    // 0 = -f(dt/dlambd)^2 + 1/f * (dr/dlambd)^2 + r^2(dphi/dlambd)^2 
    // Solved for dt/dlambd:
    // dt/dlambd = sqrt( dr^2/f^2 + r^2dphi^2/f )
    double f = 1.0 - (bh->schwarz_r / ray->r);  // Schwarzchild metric factor (0 at event horizon, 1 infinately far away)
    ray->L = ray->r * ray->r * ray->dphi;
    double dt_dlambd = sqrt(
        (ray->dr * ray->dr) / (f * f) + 
        (ray->r * ray->r * ray->dphi * ray->dphi) / f
    );
    ray->E = f * dt_dlambd;

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

    ray_trail_push(ray, (float)ray->x, (float)ray->y);
    return 0;
}

void ray_step(Ray* ray, BlackHole* bh, double dt_seconds, double time_scale) {
    if (!ray) return;
    if (ray->r <= bh->schwarz_r) return;  // Don't step if within the event horizon

    // Updates r & phi using the geodesic equations (affine increment in seconds)
    const double scaled_dt = dt_seconds * time_scale;
    GeodesicState next = rk4Step(ray, bh->schwarz_r, scaled_dt);

    ray->r = next.r;
    ray->phi = next.phi;
    ray->dr = next.dr;
    ray->dphi = next.dphi;
    ray->x = bh->x + cos(ray->phi) * ray->r;
    ray->y = bh->y + sin(ray->phi) * ray->r;

    // Push tail
    ray_trail_push(ray, (float)ray->x, (float)ray->y);
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

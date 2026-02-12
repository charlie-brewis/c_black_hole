#include "constants.h"
#include <math.h>

void geodesic(Ray* ray, double schwarz_r, double dgam) {
    const double r = ray->r;
    const double phi = ray->phi;
    const double vx = ray->x_ang * C;
    const double vy = ray->y_ang * C;
    double dr = (ray->x_pos * vx + ray->y_pos * vy) / ray->r;
    double dphi = (ray->x_pos * vy - ray->y_pos * vx) / (ray->r * ray->r);

    const double ddr = r * dphi*dphi - C*C * schwarz_r / (2.0 * r*r);
    const double ddphi = -2.0 * dr * dphi / r;

    dr += ddr * dgam;
    dphi += ddphi * dgam;

    ray->r += dr * dgam;
    ray->phi += dphi * dgam;

    // Update direction from the integrated polar derivatives.
    const double cos_phi = cos(ray->phi);
    const double sin_phi = sin(ray->phi);
    const double vx_p = dr * cos_phi - ray->r * sin_phi * dphi;
    const double vy_p = dr * sin_phi + ray->r * cos_phi * dphi;
    const double mag = hypot(vx_p, vy_p);
    if (mag > 0.0) {
        ray->x_ang = vx_p / mag;
        ray->y_ang = vy_p / mag;
    }
    
    // Update position from the integrated polar derivatives
    ray->x_pos = cos(ray->phi) * ray->r;
    ray->y_pos = sin(ray->phi) * ray->r;
}

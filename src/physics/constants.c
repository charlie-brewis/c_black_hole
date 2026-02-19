#include "constants.h"
#include <math.h>

static void geodesic_derivs(const GeodesicState* state, double schwarz_r, double E, double out[4]) {
    const double r = state->r;
    const double dphi = state->dphi;
    const double dr = state->dr;
    const double f = 1.0 - schwarz_r / r;
    const double dt_dlambda = E / f;

    // State derivatives: [r, phi, dr, dphi]
    out[0] = dr;
    out[1] = dphi;
    // FULL Schwarzchild null radial equation
    out[2] = 
        - (schwarz_r / (2.0 * r * r)) * f * (dt_dlambda * dt_dlambda)
        + (schwarz_r / (2.0 * r * r * f)) * (dr * dr)
        + (r - schwarz_r) * (dphi * dphi);
    out[3] = -2.0 * dr * dphi / r;
}

static GeodesicState geodesic_state_add(const GeodesicState* state, const double derivs[4], double scale) {
    GeodesicState next = {
        .r = state->r + scale * derivs[0],
        .phi = state->phi + scale * derivs[1],
        .dr = state->dr + scale * derivs[2],
        .dphi = state->dphi + scale * derivs[3],
    };
    return next;
}

/*
 * Solves the einstein geodesic field equation using the Schwarzchild Method
 * I.e., calculates the shortest path for light on a curved surface  
 *
 * This is an integration using Euler's method, meaning it calculates 
 * derivatives in a constant step size (dgam) and applies them to approximate.
*/
GeodesicState geodesic(Ray* ray, double schwarz_r, double dgam) {
    GeodesicState state = {
        .r = ray->r,
        .phi = ray->phi,
        .dr = ray->dr,
        .dphi = ray->dphi,
    };
    double derivs[4];
    geodesic_derivs(&state, schwarz_r, ray->E, derivs);

    double dr = state.dr;
    double dphi = state.dphi;
    const double ddr = derivs[2];
    const double ddphi = derivs[3];

    dr += ddr * dgam;
    dphi += ddphi * dgam;

    // Assign new values back to state
    state.r += dr * dgam;
    state.phi += dphi * dgam;
    state.dr = dr;
    state.dphi = dphi;

    return state;
}

/*
 * Runge-Kutta 4 is essentially applying Euler's method for
 * the next 4 steps and averaging the results. This is
 * will give us much more realisitc motion than only single-step.
*/ 
GeodesicState rk4Step(Ray* ray, double schwarz_r, double dgam) {
    GeodesicState y0 = {
        .r = ray->r,
        .phi = ray->phi,
        .dr = ray->dr,
        .dphi = ray->dphi,
    };

    double k1[4], k2[4], k3[4], k4[4];
    geodesic_derivs(&y0, schwarz_r, ray->E, k1);

    const GeodesicState y1 = geodesic_state_add(&y0, k1, 0.5 * dgam);
    geodesic_derivs(&y1, schwarz_r, ray->E, k2);

    const GeodesicState y2 = geodesic_state_add(&y0, k2, 0.5 * dgam);
    geodesic_derivs(&y2, schwarz_r, ray->E, k3);

    const GeodesicState y3 = geodesic_state_add(&y0, k3, dgam);
    geodesic_derivs(&y3, schwarz_r, ray->E, k4);

    const double inv6 = 1.0 / 6.0;
    const double r_next    = y0.r    + dgam * inv6 * (k1[0] + 2.0 * k2[0] + 2.0 * k3[0] + k4[0]);
    const double phi_next  = y0.phi  + dgam * inv6 * (k1[1] + 2.0 * k2[1] + 2.0 * k3[1] + k4[1]);
    const double dr_next   = y0.dr   + dgam * inv6 * (k1[2] + 2.0 * k2[2] + 2.0 * k3[2] + k4[2]);
    const double dphi_next = y0.dphi + dgam * inv6 * (k1[3] + 2.0 * k2[3] + 2.0 * k3[3] + k4[3]);

    GeodesicState next = {
        .r = r_next,
        .phi = phi_next,
        .dr = dr_next,
        .dphi = dphi_next,
    };

    return next;
}

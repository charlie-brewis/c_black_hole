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

#pragma once

#include "physics/constants.h"

// Obviuosly we don't want to render the stellar objects at full render_scale
// So we render them to a target_ndc_radius and scale that dependent on a relative reference mass
// I have chosen the mass of SagA as our reference mass
#define REFERENCE_MASS 8.54e36
#define TARGET_NDC_RADIUS 0.15

static inline double schwarzschild_radius(double mass) {
    return (2 * G * mass) / (C * C);
}

static inline double render_scale(void) {
    return TARGET_NDC_RADIUS / schwarzschild_radius(REFERENCE_MASS);
}

static inline double ndc_to_meters(double ndc) {
    return ndc / render_scale();
}

#pragma once

#include "ray/ray.h"
#define G 6.67430e-11
#define C 299792458.0

typedef struct {
    double r;
    double phi;
    double dr;
    double dphi;
} GeodesicState;

GeodesicState geodesic(Ray* ray, double schwarz_r, double dgam);
GeodesicState rk4Step(Ray* ray, double schwarz_r, double dgam);

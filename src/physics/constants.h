#pragma once

#include "ray/ray.h"
#define G 6.67430e-11
#define C 299792458.0

/*
 * Solves the einstein geodesic field equation using the Schwarzchild Method
 * I.e., calculates the shortest path for light on a curved surface
*/
void geodesic(Ray* ray, double schwarz_r, double dgam);

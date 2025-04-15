#ifndef STL_TRIANGULO_H
#define STL_TRIANGULO_H

#include "Vector.h"
#include "Rayo.h"

typedef struct {
    VEC3_T v0, v1, v2; 
} TRIANGLE_T;

HitInfo triangulo(RAY_T ray, TRIANGLE_T tri); 
float shading_triangle(HitInfo hit, VEC3_T light);

#endif
#ifndef RAYO_H
#define RAYO_H
#define SKY 1e8

#include "Vector.h"

typedef struct {
    VEC3_T o, d;
} RAY_T;

typedef struct {
    int hit;       
    float t;        
    VEC3_T point;     
    VEC3_T normal;    
} HitInfo;

#endif
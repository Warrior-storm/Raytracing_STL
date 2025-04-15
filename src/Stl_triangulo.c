#include "Stl_triangulo.h"
#include <math.h>

HitInfo triangulo(RAY_T ray, TRIANGLE_T tri) {
    const float EPSILON = 1e-6;
    HitInfo hit = {0};
    VEC3_T e1 = VEC3_sub(tri.v1, tri.v0);
    VEC3_T e2 = VEC3_sub(tri.v2, tri.v0);

    VEC3_T h = VEC3_cross(ray.d, e2);

    float a = VEC3_dot(e1, h);

    if (fabs(a) < EPSILON) return hit;

    float f = 1.0 / a;
    VEC3_T s = VEC3_sub(ray.o, tri.v0);
    float u = f * VEC3_dot(s, h); 

    if (u < 0.0 || u > 1.0) return hit; 

    VEC3_T q = VEC3_cross(s, e1);

    float v = f * VEC3_dot(ray.d, q);

    if (v < 0.0 || u + v > 1.0) return hit;

    float t = f * VEC3_dot(e2, q); 


    if (t > EPSILON) { 
        hit.hit = 1; 
        hit.t = t;
        hit.point = VEC3_add(ray.o, VEC3_scale(ray.d, t));
        hit.normal = VEC3_cross(e1, e2);
    }

    return hit;
}

float shading_triangle(HitInfo hit, VEC3_T light) {
    if (!hit.hit)
        return 0.0f;

    VEC3_T normal = VEC3_normalize(hit.normal);
    light = VEC3_normalize(light);

    float diffuse = VEC3_dot(normal, light);

    if (diffuse < 0.0f)
        diffuse = 0.0f;

    return diffuse;
}


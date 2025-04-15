#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "src/Stl_triangulo.h"
#include "src/Vector.h"

#define SIDE 512
#define SCALE 255
#define FRAMES 60

VEC3_T Camara[FRAMES];

void generar_posiciones_camara() {
    float radius = 3.0f;
    for (int i = 0; i < FRAMES; ++i) {
        float angle = 2.0f * M_PI * i / FRAMES;
        Camara[i] = (VEC3_T){radius * cosf(angle), radius * sinf(angle), 0.0f};
    }
}

TRIANGLE_T* cargar_stl(const char* filename, int* num_tris) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Error abriendo STL");
        exit(EXIT_FAILURE);
    }

    fseek(file, 80, SEEK_SET);
    fread(num_tris, sizeof(int), 1, file);
    printf("Triángulos: %d\n", *num_tris);

    TRIANGLE_T* tris = malloc(sizeof(TRIANGLE_T) * (*num_tris));
    if (!tris) {
        perror("Memoria insuficiente");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < *num_tris; ++i) {
        float n[3], v0[3], v1[3], v2[3];
        fread(n, sizeof(float), 3, file);
        fread(v0, sizeof(float), 3, file);
        fread(v1, sizeof(float), 3, file);
        fread(v2, sizeof(float), 3, file);
        fseek(file, 2, SEEK_CUR); 

        tris[i] = (TRIANGLE_T){
            .v0 = {v0[0], v0[1], v0[2]},
            .v1 = {v1[0], v1[1], v1[2]},
            .v2 = {v2[0], v2[1], v2[2]}
        };
    }

    fclose(file);
    return tris;
}

float Iluminacion(HitInfo hit, VEC3_T light, VEC3_T view_dir) {
    VEC3_T normal = VEC3_normalize(hit.normal);
    VEC3_T light_dir = VEC3_normalize(light);

    float ambient = 0.1f;

    float diffuse = fmaxf(0.0f, VEC3_dot(normal, light_dir));

    VEC3_T reflect_dir = VEC3_normalize(VEC3_sub(VEC3_scale(normal, 2.0f * VEC3_dot(normal, light_dir)), light_dir));
    float specular = powf(fmaxf(0.0f, VEC3_dot(reflect_dir, view_dir)), 32.0f);

    float intensity = ambient + diffuse + 0.5f * specular;
    return fminf(intensity, 1.0f);
}

int main(void) {
    generar_posiciones_camara();

    int num_tris;
    TRIANGLE_T* tris = cargar_stl("assets/Simplificado Final.stl", &num_tris);

    for (int frame = 0; frame < FRAMES; ++frame) {
        char filename[32];
        sprintf(filename, "frames2/ESCENA_%02d.pgm", frame);
        FILE* file = fopen(filename, "w");
        if (!file) {
            perror("Error al crear archivo");
            free(tris);
            return EXIT_FAILURE;
        }

        RAY_T ray = { .o = Camara[frame]};
        VEC3_T view_dir = VEC3_normalize(VEC3_sub(ray.o, (VEC3_T){0, 0, 0}));
        VEC3_T light = (VEC3_T){1.0f, 1.0f, -1.0f};

        VEC3_T up = {0, 0, 1};
        VEC3_T right = VEC3_normalize(VEC3_cross(view_dir, up));
        VEC3_T real_up = VEC3_cross(right, view_dir);
        float plane_dist = 1.0f;
        VEC3_T center = VEC3_sub(ray.o, VEC3_scale(view_dir, plane_dist));

        fprintf(file, "P2\n%d %d\n%d\n", SIDE, SIDE, SCALE);
        for (int i = 0; i < SIDE; ++i) {
            for (int j = 0; j < SIDE; ++j) {
                float u = (j - SIDE / 2.0f) / SIDE;
                float v = (i - SIDE / 2.0f) / SIDE;

                VEC3_T offset = VEC3_add(
                    VEC3_scale(right, u * 2.0f),
                    VEC3_scale(real_up, v * 2.0f)
                );

                VEC3_T pixel_pos = VEC3_add(center, offset);
                ray.d = VEC3_normalize(VEC3_sub(pixel_pos, ray.o)); 

                float closest_t = INFINITY;
                HitInfo closest_hit = {0};

                for (int k = 0; k < num_tris; ++k) {
                    HitInfo hit = triangulo(ray, tris[k]);
                    if (hit.hit && hit.t < closest_t) {
                        closest_t = hit.t;
                        closest_hit = hit;
                    }
                }

                int intensity = 0;
                if (closest_hit.hit) {
                    intensity = (int)(Iluminacion(closest_hit, light, view_dir) * SCALE);
                }

                fprintf(file, "%d ", intensity);
            }
            fprintf(file, "\n"); 
        }

        fclose(file);
    }

    free(tris);
    return EXIT_SUCCESS;
}
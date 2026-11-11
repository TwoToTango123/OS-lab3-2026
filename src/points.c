#include "points.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

Point *generate_points(size_t n, unsigned int seed) {
    Point *p = malloc(n * sizeof(Point));
    if (p == NULL) {
        perror("malloc");
        return NULL;
    }
    srand(seed);
    for (size_t i = 0; i < n; i++) {
        p[i].x = (double)rand() / RAND_MAX * 2000.0 - 1000.0;
        p[i].y = (double)rand() / RAND_MAX * 2000.0 - 1000.0;
        p[i].z = (double)rand() / RAND_MAX * 2000.0 - 1000.0;
    }
    return p;
}

Point *read_points(const char *filename, size_t *n) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        perror(filename);
        return NULL;
    }

    long count;
    if (fscanf(f, "%ld", &count) != 1 || count <= 0) {
        fprintf(stderr, "%s: bad number of points\n", filename);
        fclose(f);
        return NULL;
    }

    Point *p = malloc((size_t)count * sizeof(Point));
    if (p == NULL) {
        perror("malloc");
        fclose(f);
        return NULL;
    }

    for (long i = 0; i < count; i++) {
        if (fscanf(f, "%lf %lf %lf", &p[i].x, &p[i].y, &p[i].z) != 3) {
            fprintf(stderr, "%s: can't read point number %ld\n", filename, i + 1);
            free(p);
            fclose(f);
            return NULL;
        }
    }

    fclose(f);
    *n = (size_t)count;
    return p;
}

double triangle_area(Point a, Point b, Point c) {
    double ux = b.x - a.x;
    double uy = b.y - a.y;
    double uz = b.z - a.z;
    double vx = c.x - a.x;
    double vy = c.y - a.y;
    double vz = c.z - a.z;

    double cx = uy * vz - uz * vy;
    double cy = uz * vx - ux * vz;
    double cz = ux * vy - uy * vx;

    return sqrt(cx * cx + cy * cy + cz * cz) / 2.0;
}

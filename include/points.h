#ifndef POINTS_H
#define POINTS_H

#include <stddef.h>

typedef struct {
    double x;
    double y;
    double z;
} Point;

Point *generate_points(size_t n, unsigned int seed);

Point *read_points(const char *filename, size_t *n);

double triangle_area(Point a, Point b, Point c);

#endif

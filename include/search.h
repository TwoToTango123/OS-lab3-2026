#ifndef SEARCH_H
#define SEARCH_H

#include "points.h"

#include <stddef.h>

typedef struct {
    size_t i;
    size_t j;
    size_t k;
    double value;
} Triangle;

int find_max_triangle(const Point *points, size_t n, int threads, Triangle *result);

#endif

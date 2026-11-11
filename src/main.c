#include "points.h"
#include "search.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

static void usage(const char *prog) {
    fprintf(stderr, "usage: %s -t threads (-n count [-s seed] | -f file)\n", prog);
    fprintf(stderr, "  -t  max number of threads\n");
    fprintf(stderr, "  -n  generate count random points\n");
    fprintf(stderr, "  -s  seed for random generator (default 1)\n");
    fprintf(stderr, "  -f  read points from file\n");
}

static long parse_number(const char *str, const char *name) {
    char *end;
    errno = 0;
    long value = strtol(str, &end, 10);
    if (errno != 0 || *end != '\0' || end == str || value <= 0) {
        fprintf(stderr, "bad value for %s: %s\n", name, str);
        exit(EXIT_FAILURE);
    }
    return value;
}

static void print_point(const char *name, size_t index, Point p) {
    printf("  %s = #%zu (%.3f, %.3f, %.3f)\n", name, index, p.x, p.y, p.z);
}

int main(int argc, char *argv[]) {
    int threads = 0;
    long count = 0;
    unsigned int seed = 1;
    char *filename = NULL;

    int opt;
    while ((opt = getopt(argc, argv, "t:n:s:f:")) != -1) {
        switch (opt) {
        case 't':
            threads = (int)parse_number(optarg, "-t");
            break;
        case 'n':
            count = parse_number(optarg, "-n");
            break;
        case 's':
            seed = (unsigned int)parse_number(optarg, "-s");
            break;
        case 'f':
            filename = optarg;
            break;
        default:
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (threads <= 0 || (count == 0 && filename == NULL) || (count != 0 && filename != NULL)) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    size_t n;
    Point *points;
    if (filename != NULL) {
        points = read_points(filename, &n);
    } else {
        n = (size_t)count;
        points = generate_points(n, seed);
    }
    if (points == NULL) {
        return EXIT_FAILURE;
    }

    if (n < 3) {
        fprintf(stderr, "need at least 3 points, got %zu\n", n);
        free(points);
        return EXIT_FAILURE;
    }

    printf("main: PID=%d, points=%zu, max threads=%d\n", getpid(), n, threads);

    struct timespec start, end;
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock_gettime");
        free(points);
        return EXIT_FAILURE;
    }

    Triangle res;
    if (find_max_triangle(points, n, threads, &res) != 0) {
        free(points);
        return EXIT_FAILURE;
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        perror("clock_gettime");
        free(points);
        return EXIT_FAILURE;
    }

    double ms = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;

    printf("max triangle:\n");
    print_point("A", res.i, points[res.i]);
    print_point("B", res.j, points[res.j]);
    print_point("C", res.k, points[res.k]);
    printf("area: %.6f\n", triangle_area(points[res.i], points[res.j], points[res.k]));
    printf("time: %.3f ms\n", ms);

    free(points);
    return EXIT_SUCCESS;
}

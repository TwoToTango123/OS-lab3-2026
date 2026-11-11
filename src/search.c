#include "search.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    int num;
    long rows;
} ThreadArg;

static const Point *pts;
static size_t pts_count;
static size_t next_row;
static Triangle best;
static pthread_mutex_t row_mutex;
static pthread_mutex_t best_mutex;

static void lock(pthread_mutex_t *m) {
    int err = pthread_mutex_lock(m);
    if (err != 0) {
        fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(err));
        exit(EXIT_FAILURE);
    }
}

static void unlock(pthread_mutex_t *m) {
    int err = pthread_mutex_unlock(m);
    if (err != 0) {
        fprintf(stderr, "pthread_mutex_unlock: %s\n", strerror(err));
        exit(EXIT_FAILURE);
    }
}

static void *worker(void *arg) {
    ThreadArg *a = arg;
    printf("thread %d: TID=%d started\n", a->num, gettid());

    Triangle local;
    local.i = 0;
    local.j = 0;
    local.k = 0;
    local.value = -1.0;

    while (1) {
        lock(&row_mutex);
        size_t i = next_row;
        next_row++;
        unlock(&row_mutex);

        if (i + 2 >= pts_count) {
            break;
        }

        Point A = pts[i];
        for (size_t j = i + 1; j < pts_count; j++) {
            double ux = pts[j].x - A.x;
            double uy = pts[j].y - A.y;
            double uz = pts[j].z - A.z;
            for (size_t k = j + 1; k < pts_count; k++) {
                double vx = pts[k].x - A.x;
                double vy = pts[k].y - A.y;
                double vz = pts[k].z - A.z;

                double cx = uy * vz - uz * vy;
                double cy = uz * vx - ux * vz;
                double cz = ux * vy - uy * vx;
                double s = cx * cx + cy * cy + cz * cz;

                if (s > local.value) {
                    local.value = s;
                    local.i = i;
                    local.j = j;
                    local.k = k;
                }
            }
        }
        a->rows++;
    }

    lock(&best_mutex);
    if (local.value > best.value || (local.value == best.value && local.i < best.i)) {
        best = local;
    }
    unlock(&best_mutex);

    printf("thread %d: TID=%d finished, rows=%ld\n", a->num, gettid(), a->rows);
    return NULL;
}

int find_max_triangle(const Point *points, size_t n, int threads, Triangle *result) {
    pts = points;
    pts_count = n;
    next_row = 0;
    best.i = 0;
    best.j = 0;
    best.k = 0;
    best.value = -1.0;

    int err = pthread_mutex_init(&row_mutex, NULL);
    if (err != 0) {
        fprintf(stderr, "pthread_mutex_init: %s\n", strerror(err));
        return -1;
    }
    err = pthread_mutex_init(&best_mutex, NULL);
    if (err != 0) {
        fprintf(stderr, "pthread_mutex_init: %s\n", strerror(err));
        pthread_mutex_destroy(&row_mutex);
        return -1;
    }

    pthread_t *ids = malloc((size_t)threads * sizeof(pthread_t));
    ThreadArg *args = malloc((size_t)threads * sizeof(ThreadArg));
    if (ids == NULL || args == NULL) {
        perror("malloc");
        free(ids);
        free(args);
        pthread_mutex_destroy(&row_mutex);
        pthread_mutex_destroy(&best_mutex);
        return -1;
    }

    int ret = 0;
    int created = 0;
    for (int t = 0; t < threads; t++) {
        args[t].num = t + 1;
        args[t].rows = 0;
        err = pthread_create(&ids[t], NULL, worker, &args[t]);
        if (err != 0) {
            fprintf(stderr, "pthread_create: %s\n", strerror(err));
            ret = -1;
            break;
        }
        created++;
    }

    for (int t = 0; t < created; t++) {
        err = pthread_join(ids[t], NULL);
        if (err != 0) {
            fprintf(stderr, "pthread_join: %s\n", strerror(err));
            ret = -1;
        }
    }

    free(ids);
    free(args);
    pthread_mutex_destroy(&row_mutex);
    pthread_mutex_destroy(&best_mutex);

    if (ret == 0) {
        *result = best;
    }
    return ret;
}

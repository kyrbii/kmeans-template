#include "distance.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>

double compute_manhattan_distance(int d, const double x1[d], const double x2[d]) {
    double aggr_distance = 0.;
    for (int i = 0; i < d; i++) {
        aggr_distance += fabs(x1[i] - x2[i]);
    }
    return aggr_distance;
}

double compute_euclidean_distance(int d, const double x1[d], const double x2[d]) {
    double aggr_distance = 0.;
    for (int i = 0; i < d; i++) {
        aggr_distance += (x1[i] - x2[i]) * (x1[i] - x2[i]);
    }
    return aggr_distance;
}

double compute_distance(int d, const double x1[d], const double x2[d], distance dist) {
    switch (dist) {
        case MANHATTAN:
            return compute_manhattan_distance(d, x1, x2);
        case EUCLIDEAN:
            return compute_euclidean_distance(d, x1, x2);
        default:
            assert("there's a type of distance I don't know yet.");

    }
}


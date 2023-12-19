#include <assert.h>
#include <stdio.h>
#include <stdlib.h> // exit
#include <string.h>
#include <stdbool.h>
#include "distance.h"
#include "ioutils.h"

void initialize_cluster_assignment(int n, int y[n], int value) {
    for (int i = 0; i < n; i++) {
        y[i] = value;
    }
}

void copy_cluster_assignment(int n, const int from_y[n], int to_y[n]) {
    for (int i = 0; i < n; i++) {
        to_y[i] = from_y[i];
    }
}

bool is_identical_cluster_assignment(int n, const int a[n], const int b[n]) {
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

/* return a truly uniform random value in the range 0..n-1 inclusive */
int rand_in_range(int n) {
    int limit;
    int r;
    limit = RAND_MAX - (RAND_MAX % n);
    while((r = rand()) >= limit);
    return r % n;
}

bool tests() {
    // either have very many assert statements in here, or call more fnuctions which test via assert statements
    return true;
}

void check_usage(int argc, char** argv) {
    if (argc != 6) {
        printf("usage: %s <n> <d> <filename.csv> <k> <distance-metric>\n", argv[0]);
        printf("<n> number of data points to be read\n");
        printf("<d> dimensionality of data points\n");
        printf("<filename.csv> contains the data to be classified (1st line will be ignored)\n");
        printf("<k> number of neighbours for majority voting\n");
        printf("<distance-metric>: must be \"manhattan\" or \"euclidean\"\n");
        exit(1);
    }
}

int main(int argc, char** argv) {
    assert(tests() && printf("tests have passed\n"));
    srand(1);
    check_usage(argc, argv);
    int n = atoi(argv[1]);
    int d = atoi(argv[2]);
    char* filename = argv[3];
    int k = atoi(argv[4]);
    distance dist = strcmp(argv[5], "manhattan") == 0 ? MANHATTAN : EUCLIDEAN;

    double x[n][d];
    int y[n];
    n = read_csv(filename, n, d, x, y);
    // printf("Original data:\n");
    // print_csv(n, d, x, y);
    initialize_cluster_assignment(n, y, -1); // overwrite clusterIDs so that cluster() can't use it.

    double centroids[k][d];

    // TODO: cluster(n, d, x, y, k, centroids, dist);

    printf("Clustering result:\n");
    for (int i = 0; i < k; i++) {
        printf("Centroid for cluster %d: ", i);
        print_datapoint(d, centroids[i]);
        printf("\n");
    }
    print_csv(n, d, x, (int*) y);
}

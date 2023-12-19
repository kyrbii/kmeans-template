#include <assert.h>
#include <stdio.h>
#include <stdlib.h> // exit
#include <string.h>
#include <stdbool.h>
#include "distance.h"
#include "ioutils.h"

void split_train_test(int test, int n, int d,
                      double full_x[n][d], const int full_y[n],
                      double train_x[n-1][d], int train_y[n-1],
                      double test_x[d], int* test_y) {
    // these are identical until we hit the test item
    int i = 0; // index over full variables
    int j = 0; // index over train variables
    while (i < n) {
        if (i == test) {
            copy_instance(d, full_x[i], test_x);
            *test_y = full_y[i];
            i++;
        } else {
            copy_instance(d, full_x[i], train_x[j]);
            train_y[j] = full_y[i];
            i++;
            j++;
        }
    }
}

// the one-best nearest-neighbour classifier is substantially simpler than knn.
// that's why I put it here as well:
int one_nn_classifier(int n, int d, double train_x[n][d], int train_y[n], double test_x[d], distance dist) {
    assert(n > 0 && "hey, I need at least one instance in the training data!");
    int closest_class = train_y[0];
    double min_distance = compute_distance(d, test_x, train_x[0], dist);
    for (int i = 1; i < n; i++) {
        double new_distance = compute_distance(d, test_x, train_x[i], dist);
        if (new_distance < min_distance) {
            min_distance = new_distance;
            closest_class = train_y[i];
        }
    }
    return closest_class;
}



/** sort the two arrays by the ordering of the first array */
void sort_ascending(int k, double best_distances[k], int best_classes[k]) {
    for (int j = k - 1; j > 1; j--) {
        for (int i = 0; i < j; i++) {
            if (best_distances[i] > best_distances[j]) {
                double temp_distance = best_distances[i];
                best_distances[i] = best_distances[j];
                best_distances[j] = temp_distance;
                int temp_class = best_classes[i];
                best_classes[i] = best_classes[j];
                best_classes[j] = temp_class;
            }
        }
    }
}

int majority_vote(int k, int best_classes[k]) {
    /* this will be a bit tricky as we do not have a map implementation.
     * what we'll do is to use the index of the first occurrence of each
     * class in best_classes as the place where we store the duplicates.
     * this yields a quadratic algorithm, but we don't run this often anyway.
     * also, we're using bubble-sort above, so quadratic seems to be okay... */
    int class_counter[k];
    class_counter[0] = 1; // we observe the class in position 0 at least this one time
    for (int i = 1; i < k; i++) {
        class_counter[i] = 0; // all others haven't been observed yet.
    }
    for (int i = 1; i < k; i++) { // now check if this is new or if we've seen it already
        int j = 0;
        while (j < i) {
            if (best_classes[i] == best_classes[j]) {
                class_counter[j]++;
                break;
            }
            j++;
        }
    }
    // now we have the number of occurrences aggregated in the positions in class_counter
    // let's go ahead and find the maximum and return the corresponding class in best_classes
    int max_count = 1; // must be at least 1
    int max_count_index = 0;
    for (int i = 0; i < k; i++) {
        if (class_counter[i] > max_count) {
            max_count = class_counter[i];
            max_count_index = i;
        }
    }
    return best_classes[max_count_index];
}

int knn_train_and_classify(int n, int d, double train_x[n][d], int train_y[n], double x[d], int k, distance dist) {
    assert(n >= k && "hey, I need at least k instances in the training data!"); // this is an artifact of the code below not a principled limitation of knn clustering (but obviously, it doesn't make sense to set k anything close to n.
    double best_distances[k];
    int best_classes[k];
    for (int i = 0; i < k; i++) {
        best_distances[i] = compute_distance(d, train_x[i], x, dist);
        best_classes[i] = train_y[i];
    }
    sort_ascending(k, best_distances, best_classes);
    for (int i = k; i < n; i++) {
        double new_distance = compute_distance(d, train_x[i], x, dist);
        if (new_distance < best_distances[k-1]) {
            best_distances[k-1] = new_distance;
            best_classes[k-1] = train_y[i];
            sort_ascending(k, best_distances, best_classes);
        }
    }
    return majority_vote(k, best_classes);
}

int loo_knn_experiment(int n, int d, double full_x[n][d], int full_y[n], int k, distance dist) {
    double train_x[n-1][d];
    int train_y[n-1];
    double test_x[d];
    int test_y;
    int correctly_classified = 0;
    for (int i = 0; i < n; i++) {
        split_train_test(i, n, d, full_x, full_y, train_x, train_y, test_x, &test_y);
        // hier kann man für k=1 natürlich auch one_nn_classifier benutzen
        // bool correct = one_nn_classifier(n-1, d, train_x, train_y, test_x, dist) == test_y;
        bool correct = knn_train_and_classify(n-1, d, train_x, train_y, test_x, k, dist) == test_y;
        correctly_classified += (int) correct;
    }
    return correctly_classified;
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
    check_usage(argc, argv);
    int n = atoi(argv[1]);
    int d = atoi(argv[2]);
    char* filename = argv[3];
    int k = atoi(argv[4]);
    distance dist = strcmp(argv[5], "manhattan") ? MANHATTAN : EUCLIDEAN;

    double x[n][d];
    int y[n];
    n = read_csv(filename, n, d, x, y);
    //print_csv(n, d, x, y);
    int correct = loo_knn_experiment(n, d, x, y, k, dist);
    printf("%d out of %d have been classified correctly (for a proportion of %f)\n", correct, n, (float) correct / n);
    return 0;
}

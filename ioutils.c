#include <stdio.h>
#include <stdlib.h> // exit

void print_datapoint(int d, const double x[d]) {
    for (int j = 0; j < d - 1; j++) {
        printf("%lf, ", x[j]);
    }
    printf("%lf", x[d - 1]);
}

void print_csv(int n, int d, const double x[n][d], const int y[n]) {
    for (int i = 0; i < n; i++) {
        print_datapoint(d, x[i]);
        printf(", %d\n", y[i]);
    }
}

/**c
 * reads up to n items from a given file and fills them into x and y
 * @param n the (maximum) number of items to be read from CSV file
 * @param d the dimensionality of the items (plus one more for y)
 * @param x
 * @param y
 * @param filename
 * @return the number of items actually read (can be lower if CSV file is shorter)
 */
int read_csv(const char* filename, int n, int d, double x[n][d], int y[d]) {
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        printf("%s cannot be found", filename);
        exit(2);
    } // let's now continue as if f is not NULL (otherwise we have already aborted)
    // discard the first line (let's hope that it's not longer than that)
    // BUG ALERT: do not write this in production code but find out how to safely read a full line.
    char line[65536];
    fgets(line, 65536, f);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < d; j++) {
            int ret = fscanf(f, "%lf,", &x[i][j]);
            if (ret == EOF) {
                fclose(f);
                return i;
            }
        }
        int ret = fscanf(f, "%d", &y[i]);
        if (ret == EOF) {
            fclose(f);
            return i;
        }
    }
    fclose(f);
    return n;
}

void copy_instance(int d, const double src[d], double tgt[d]) {
    for (int i = 0; i < d; i++) {
        tgt[i] = src[i];
    }
}

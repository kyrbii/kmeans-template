#include <assert.h>
#include <stdio.h>
#include <stdlib.h> // exit
#include <string.h>
#include <stdbool.h>
#include "distance.h"
#include "ioutils.h"
#include <math.h>
// compiler command for gcc if you have the files in the current folder
// gcc -o .\cluster.exe .\Clustering.c .\distance.c .\ioutils.c

void initialize_cluster_assignment(int n, int y[n], int value)
{
    for (int i = 0; i < n; i++)
    {
        y[i] = value;
    }
}

void copy_cluster_assignment(int n, const int from_y[n], int to_y[n])
{
    for (int i = 0; i < n; i++)
    {
        to_y[i] = from_y[i];
    }
}

bool is_identical_cluster_assignment(int n, const int a[n], const int b[n])
{
    for (int i = 0; i < n; i++)
    {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

/* return a truly uniform random value in the range 0..n-1 inclusive */
int rand_in_range(int n)
{
    int limit;
    int r;
    limit = RAND_MAX - (RAND_MAX % n);
    while ((r = rand()) >= limit)
        ;
    return r % n;
}

bool tests()
{
    // either have very many assert statements in here, or call more fnuctions which test via assert statements
    return true;
}

void check_usage(int argc, char **argv)
{
    if (argc != 6)
    {
        printf("usage: %s <n> <d> <filename.csv> <k> <distance-metric>\n", argv[0]);
        printf("<n> number of data points to be read\n");
        printf("<d> dimensionality of data points\n");
        printf("<filename.csv> contains the data to be classified (1st line will be ignored)\n");
        printf("<k> number of clusters\n");
        printf("<distance-metric>: must be \"manhattan\" or \"euclidean\"\n");
        exit(1);
    }
}

bool comp_indices(int indices, int n, int d, int k, int number, double centroids[k][d], double x[n][d]) // schaut, ob ein centroid auf einen Punkt gesetzt werden soll, auf dem bereits ein centroid ist
{
    int dummy;
    for (size_t i = 0; i < indices; i++)
    {
        dummy = 0;
        for (size_t f = 0; f < d; f++)
        {
            if (centroids[i][f] == x[number][f])
            {
                dummy++;
            }
        }
        if (dummy == d)
        {
            return true;
        }
    }

    return false;
}

void centroid_init(int indices, int n, int d, int k, double centroids[k][d], double x[n][d])
{
    int number;
    do
    {
        number = rand_in_range(n);                                  // Weise einen Zufallswert zu um auf dem zugehoerigen datensatz einen centroid zu machen
    } while (comp_indices(indices, n, d, k, number, centroids, x)); // Schaut ob wir den Datenpunkt schonmal hergenommen haben. Wenn schon, dann das Ganze nochmal
    for (size_t i = 0; i < d; i++)
    {
        centroids[indices][i] = x[number][i]; // centroid auf den Datenpunkt x legen
    }
}

void assign_to_centroid(int n, int d, int k, double centroids[k][d], double x[n][d], int y[n], distance dist) // Ordne den Datnesaetzen einen Centroid zu
{
    long double temp_distance;
    long double shortest_distance;
    int centroid_rem;

    for (size_t v = 0; v < n; v++) // Fuer jeden Datensatz
    {
        centroid_rem = 0;
        shortest_distance = INFINITY;
        temp_distance = 1;
        for (size_t i = 0; i < k; i++) // Einmal jeden Centroid anschauen
        {
            temp_distance = compute_distance(d, centroids[i], x[v], dist); // Die distanz von einem Datenpunkt zum Centroid ausrechnen
            if (temp_distance < shortest_distance)                         // Schauen ob sie kleiner ist, als eine bereits errechnete Distanz zu einem anderen Centroid
            {
                shortest_distance = temp_distance;
                centroid_rem = i; // indices des gemerkten centroids merken
            }
        }
        y[v] = centroid_rem; // in y den zugeordneten Centroid schreiben#
    }
}

void move_centroid(int n, int d, int k, double centroids[k][d], double x[n][d], int y[n]) // verschiebt den centroid in den Mittelpunkt, der Ihm zugeordneten Satensaetzen
{
    long double dimension_avg; // Je nachdem wie hoch die Distanzen werden koennen
    double datapoint_num = 0;

    for (size_t i = 0; i < k; i++) // Fuer jeden Centroid
    {
        for (size_t b = 0; b < d; b++) // Fuer jede Dimension
        {
            dimension_avg = 0; // Null setzen, dass des Ergebnis der vorherigen Dimension nicht verrechnet wird
            datapoint_num = 0;

            for (size_t p = 0; p < n; p++) // Fuer jeden Datenpunkt
            {
                if (y[p] == i) // Nur zusammenaddieren, wenn in y der zugehoerige centroid steht
                {
                    dimension_avg += x[p][b]; // Die Koordinaten zusammenaddieren
                    datapoint_num++;
                }
            }
            if (datapoint_num != 0)
            {
                dimension_avg = dimension_avg / datapoint_num; // Den Durchschnitt berechnen

                centroids[i][b] = dimension_avg; // Den Centroid updaten
            }
            if (datapoint_num == 0)
            {
                printf("ERROR DIVISION BY ZERO in dimension %d von Centroid %d\n", b, i);
            }
        }
    }
}

int count_doubles(int n, int d, int k, int *ptr_to_k, double x[n][d]) // Testet Datensatz auf gleiche Punkte und verringert k um die Anzahl an Daten-Paaren
{
    int dummy;
    int double_counter = 0;
    for (size_t i = 0; i < n; i++)
    {
        for (size_t g = 0; g < n; g++)
        {
            dummy = 0;
            for (size_t z = 0; z < d; z++)
            {
                if (i != g)
                {
                    if (x[i][z] == x[g][z])
                    {
                        dummy++;
                    }
                }
            }
            if (dummy == d)
            {
                double_counter++;
            }
        }
    }
    if (*ptr_to_k > (n - (double_counter / 2)))
    {
        *ptr_to_k = n - (double_counter / 2); // mit speicheraddresse von K
        k = *ptr_to_k;
    }
    return *ptr_to_k;
}

void cluster(int n, int d, double x[n][d], int y[n], int k, double centroids[k][d], distance dist, int *ptr_to_k)
{
    int y_mem[n];    // Fuer das Vergleichen, ob sich die Zuordnung zu den Centroids verändert
    int counter = 0; // Counter fuer das ^^
    *ptr_to_k = count_doubles(n, d, k, ptr_to_k, x);
    for (size_t i = 0; i < *ptr_to_k; i++)
    {                                                    // Fuer alle centroids
        centroid_init(i, n, d, *ptr_to_k, centroids, x); // centroids initialisieren
    }
    do
    {
        copy_cluster_assignment(n, y, y_mem);                       // y in y_mem kopieren um das Abbruchkriterium abzuwaegen
        assign_to_centroid(n, d, *ptr_to_k, centroids, x, y, dist); // Datensaetze den Centroids zuordnen, hier wird y neu beschrieben
        move_centroid(n, d, *ptr_to_k, centroids, x, y);            // aendere die Koordinaten/Ort des centroids
        if (is_identical_cluster_assignment(n, y, y_mem))           // falls sich y nicht veraendert hat
        {
            counter++; // Abbruchkriterium erhöhen
        }
        else
            counter = 0; // Kein Problem

    } while (counter != 2);
}

int main(int argc, char **argv)
{
    assert(tests() && printf("tests have passed\n")); // Tests die ich gerne machen würde
    srand(1);                                         // später mit time.h ersetzten, momentan zum Testen noch besser so, da immer gleiches Ergebnis
    check_usage(argc, argv);                          // schaut ob die argv eingabe rechtens war
    int n = atoi(argv[1]);                            // from string to int --> normale Zahl aus dem ASCII Wert machen
    int d = atoi(argv[2]);                            // ^^
    char *filename = argv[3];
    int k = atoi(argv[4]); // Anzahl der CLuster

    distance dist = strcmp(argv[5], "manhattan") == 0 ? MANHATTAN : EUCLIDEAN;

    double x[n][d];
    int y[n];
    n = read_csv(filename, n, d, x, y);

    initialize_cluster_assignment(n, y, -1); // overwrite clusterIDs so that cluster() can't use it.

    double centroids[k][d];

    cluster(n, d, x, y, k, centroids, dist, &k);

    printf("Clustering result:\n");
    for (int i = 0; i < k; i++)
    {
        printf("Centroid for cluster %d: ", i);
        print_datapoint(d, centroids[i]); // Pointergeschichte noch nachschauen
        printf("\n");
    }
    print_csv(n, d, x, (int *)y);
}

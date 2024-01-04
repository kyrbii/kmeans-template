#include <assert.h>
#include <stdio.h>
#include <stdlib.h> // exit
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include "distance.h"
#include "ioutils.h"

//gcc -o .\main.exe .\main.c .\distance.c .\ioutils.c;

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

bool comp_coords(int indices, int n, int d, int k, int number, double centroids[k][d], double x[n][d]) // schaut, ob ein centroid auf einen Punkt gesetzt werden soll, auf dem bereits ein centroid ist
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
    } while (comp_coords(indices, n, d, k, number, centroids, x)); // Schaut ob wir den Datenpunkt schonmal hergenommen haben. Wenn schon, dann das Ganze nochmal
    for (size_t i = 0; i < d; i++)
    {
        centroids[indices][i] = x[number][i]; // centroid auf den Datenpunkt x legen
    }
}

int clustering(int n, int d, double x[n][d], int y[n], int k, double centroids[k][d], int dist) {

    // Seid ihr euch sicher, dass ihr die Aufgabe verstanden habt? Wenn ja, wie viel Zeit habt ihr in dieses Programm gesteckt. Ich hoffe nicht so viel, so sieht es nämlich aus. sry :/
    // Im Code ist momentan keine durchgängige logische Struktur zu erkennen, was auch auf zu wenig "Gedanken machen" davor hinweist
    // Mein Tipp: schaut euch das an was ich hier kommentiert hab und sagt bitte, wenn ich einen scheiß geschrieben hab, kommt vor ich bin auf gar keinen Fall ein Ass in C :)
    // Am Besten: schreibt das Programm neu. macht euch einen Plan, was ihr erreichen wollt und wie ihr das machen wollt und zwar bevor ihr anfangt zu programmieren
    // Das ist das wodurch meine Programme gut werden
    // Des ist scheisse und macht keinen spaß des nochmal zu schreiben, aber so wie es jetzt ist, ist es nicht gut und für euch vielleicht gut zu lernen, dass das mit dem Planen was hilft
    // Sorry wenn das ein wenig hart klingt. Musste das vor ein paar Wochen am eigenen Leib herausfinden.

    //Was ihr tatsächlich macht ist euch überlassen, wenn ihr Probleme oder fragen habt, helf ich auch gerne. Will mich aber nicht als unfehlbaren C-Master darstellen


    //initialisierung von k-Zufälligen Zentroiden pro Cluster
    // TODO Hier solltet ihr noch eine überprüfung einrichten, sodass nicht zweimal der selbe Datensatz für die Initialisierung hergenommen werden
    for (size_t i = 0; i < k; i++)
    {                                            // Fuer alle centroids
        centroid_init(i, n, d, k, centroids, x); // centroids initialisieren
    }
    int y_mem[n];
    bool changed;
    int anzahl = 0;

    do {
        copy_cluster_assignment(n, y, y_mem);
        changed = false;
        // Zuweisung der Datenpunkte zu Clustern
        // (jeder Datenpunkt wird dem Zentroiden zugeordnet, der ihm am nächsten liegt)
        for (int i = 0; i < n; i++) {
            double minDistance = INT_MAX;
            int closestCentroid = -1;
            for (int j = 0; j < k; j++) {
                //Jeder Datentyp mit Zentroid Distance berechnet
                double distance;
                distance = compute_distance(d, centroids[j], x[i], dist);
                if (distance < minDistance) {
                    minDistance = distance;
                    closestCentroid = j;
                }
            }
            //Zuweisung des Datenpunkts zum nächstgelegenen Zentroiden
            y[i] = closestCentroid;
        }

        //Aktualisierung der Zentroide
        double new_centroids[k][d];
        double durchschnitt;
        double dimDurchschnitt;
        //Zusammengezählte Koordinaten pro Zentroid / Menge der Datenpunkte (clusterSize)
        double clusterSize = 0;

        for(int centr=0; centr < k; centr++) { //pro Zentroid
            clusterSize = 0;
            for (int dim = 0; dim < d; dim++) { //pro Dimension
                durchschnitt = 0; // 0 Initialisierung
                dimDurchschnitt = 0;

                for (int data = 0; data < n; data++) { //pro Datenpunkt
                    if (y[data] == centr) {
                        dimDurchschnitt += x[data][dim];
                        clusterSize++;
                    }
                }

                if (clusterSize > 0) {
                    dimDurchschnitt /= clusterSize; //Durchschnit berechnen
                    new_centroids[centr][dim] = dimDurchschnitt; //Zentroiden aktualisieren
                }else{printf("ERROR\n");}
                
                //Überprüfung ob Zentroiden sich verändert haben
                if(is_identical_cluster_assignment(n, y, y_mem)) {
                    changed = true;
                }


                centroids[centr][dim] = new_centroids[centr][dim];
            }
        }
        
        anzahl++;
    }
    while (changed); //bei while(changed) ist eine Endlosschleife

    printf("%d", anzahl);
    return 0;
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
    //printf("Original data:\n");
    //print_csv(n, d, x, y);
    initialize_cluster_assignment(n, y, -1); // overwrite clusterIDs so that cluster() can't use it.

    double centroids[k][d];

    clustering(n, d, x, y, k, centroids, dist);

    printf("Clustering result:\n");
    for (int i = 0; i < k; i++) {
        if(centroids[i]>0){
            printf("Centroid for cluster %d: ", i);
            print_datapoint(d, centroids[i]);
            printf("\n");
        }

    }
    print_csv(n, d, x, (int*) y);
}
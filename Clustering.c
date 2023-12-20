#include <assert.h>
#include <stdio.h>
#include <stdlib.h> // exit
#include <string.h>
#include <stdbool.h>
#include "distance.h"
#include "ioutils.h"
#include <math.h>

//gcc -o .\kmeans-boilerplate.exe .\kmeans-boilerplate.c .\distance.c .\ioutils.c

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
        printf("<k> number of clusters\n");
        printf("<distance-metric>: must be \"manhattan\" or \"euclidean\"\n");
        exit(1);
    }
}

void indicesmem_init(int k, int arr[k]) //init die merkindices auf minus eins, weil das spaeter ja nicht sein kann!!
{
    for (size_t i = 0; i < k; i++)  //Fuer alle Centroids
    {
        arr[i] = -1;
    }
}

bool comp_indices(int k, int number,int ind_mem[k]) //schaut, ob ein centroid auf einen Punkt gesetzt werden soll, auf dem bereits ein centroid ist
{
    for (size_t i = 0; i < k; i++)  //Fuer jeden centroid
    {
        if (number == ind_mem[i])   //schauen ob der number'te Datensatz schon hergenommen wurde
        {
            return true;
        }   
    }
    return false;
}

void centroid_init(int indices, int n, int d, int k, double centroids[k][d], double x[n][d], int ind_mem[k])
{
    int number;
    do{
    number = rand_in_range(n);  //Weise einen Zufallswert zu um auf dem zugehoerigen datensatz einen centroid zu machen
    }while(comp_indices(k, number, ind_mem));   //Schaut ob wir den Datenpunkt schonmal hergenommen haben. Wenn schon, dann das Ganze nochmal

    ind_mem[indices] = number;  //Sich den Satensatz merken, den wir jetzt hergenommen haben

    for (size_t i = 0; i < d; i++)
    {
        centroids[indices][i] = x[number][i];   //centroid auf den Datenpunkt x legen 
    }
}

void assign_to_centroid(int n, int d, int k, double centroids[k][d], double x[n][d], int y[n], distance dist)   //Ordne den Datnesaetzen einen Centroid zu
{
    double temp_distance;
    double shortest_distance;
    int centroid_rem;

    for (size_t v = 0; v < n; v++)  //Fuer jeden Datensatz
    {
        centroid_rem = 0;
        shortest_distance = INFINITY;
        temp_distance = 1;

        for (size_t i = 0; i < k; i++)  //Einmal jeden Centroid anschauen
        {
        temp_distance = compute_distance(d, centroids[i], x[n], dist);  //Die distanz von einem Datenpunkt zum Centroid ausrechnen

        if (temp_distance < shortest_distance)  //Schauen ob sie kleiner ist, als eine bereits errechnete Distanz zu einem anderen Centroid
        {
            shortest_distance = temp_distance;
            centroid_rem = i;   //indices des gemerkten centroids merken
        }

        }

        y[v] = centroid_rem;    //in y den zugeordneten Centroid schreiben
    }
}

void move_centroid(int n, int d, int k, double centroids[k][d], double x[n][d], int y[n])    //verschiebt den centroid in den Mittelpunkt, der Ihm zugeordneten Satensaetzen
{
    unsigned long long dimension_avg;   //Je nachdem wie hoch die Distanzen werden koennen

    for (size_t i = 0; i < k; i++)  //Fuer jeden Centroid
    {
        for (size_t b = 0; b < d; b++)  //Fuer jede Dimension
        {
            dimension_avg = 0;  //Null setzen, dass des Ergebnis der vorherigen Dimension nicht verrechnet wird

            for (size_t p = 0; p < n; p++)  //Fuer jeden Datenpunkt
            {
                dimension_avg += x[p][b];   //Die Koordinaten zusammenaddieren
            }
            dimension_avg = dimension_avg / n;  //Den Durchschnitt berechnen

            centroids[i][b]  = dimension_avg;   //Den Centroid updaten
        }
    }
    
}

void cluster(int n, int d, double x[n][d], int y[n], int k, double centroids[k][d], distance dist)
{
    int indices_mem[k]; //Fuer die Initialisierung der Centroids --> nicht zwei mal die gleichen Punkte
    int y_mem[n];   //Fuer das Vergleichen, ob sich die Zuordnung zu den Centroids verändert
    int counter = 0;    //Counter fuer das ^^
    indicesmem_init(k, indices_mem);    //initialisieren des Arrays
    for (size_t i = 0; i < k; i++) {    //Fuer alle centroids 
        centroid_init(i, n, d, k, centroids, x, indices_mem);   //centroids initialisieren
    }
    /*
    do
    {
        copy_cluster_assignment(n, y, y_mem);   // y in y_mem kopieren um das Abbruchkriterium abzuwaegen
        assign_to_centroid(n, d, k, centroids, x, y, dist); //Datensaetze den Centroids zuordnen, hier wird y neu beschrieben
        move_centroid(n, d, k, centroids, x, y);    //aendere die Koordinaten/Ort des centroids
        if (is_identical_cluster_assignment(n, y, y_mem))   //falls sich y nicht veraendert hat
        {
            counter++;  //Abbruchkriterium erhöhen
        }else counter = 0;  //Kein Problem
        
    } while (counter != 2);
    */
    
}

int main(int argc, char** argv) {
    assert(tests() && printf("tests have passed\n")); //Tests die ich gerne machen würde
    srand(1); //später mit time.h ersetzten, momentan zum Testen noch besser so, da immer gleiches Ergebnis
    check_usage(argc, argv); //schaut ob die argv eingabe rechtens war
    int n = atoi(argv[1]); //from string to int --> normale Zahl aus dem ASCII Wert machen
    int d = atoi(argv[2]);// ^^
    char* filename = argv[3];
    int k = atoi(argv[4]); //Anzahl der CLuster

    //distance dist = strcmp(argv[5], "manhattan") == 0 ? MANHATTAN : EUCLIDEAN;
    distance dist = EUCLIDEAN; //ACHTUNG: HIER EGAL WAS EINFEFEBEN WIRD EUCLIDEAN. NUR FUER DIESE AUFGABE

    double x[n][d];
    int y[n];
    n = read_csv(filename, n, d, x, y);
    // printf("Original data:\n");
    // print_csv(n, d, x, y);
    initialize_cluster_assignment(n, y, -1); // overwrite clusterIDs so that cluster() can't use it.

    double centroids[k][d];

    cluster(n, d, x, y, k, centroids, dist);

    printf("Clustering result:\n");
    for (int i = 0; i < k; i++) {
        printf("Centroid for cluster %d: ", i);
        print_datapoint(d, centroids[i]); //Pointergeschichte noch nachschauen
        printf("\n");
    }
    print_csv(n, d, x, (int*) y);
}
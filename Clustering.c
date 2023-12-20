#include <assert.h>
#include <stdio.h>
#include <stdlib.h> // exit
#include <string.h>
#include <stdbool.h>
#include "distance.h"
#include "ioutils.h"

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
    for (size_t i = 0; i < k; i++)
    {
        arr[i] = -1;
    }
}

bool comp_indices(int k, int number,int ind_mem[k])
{
    for (size_t i = 0; i < k; i++)
    {
        if (number == ind_mem[i])
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

    ind_mem[indices] = number;

    for (size_t i = 0; i < d; i++)
    {
        centroids[indices][i] = x[number][i];
    }
}

assign_to_centroid(n, d, k, centroids, x, y)
{

}

move_centroid(n, d, k, centroids, x, y)
{
    
}

void cluster(int n, int d, double x[n][d], int y[n], int k, double centroids[k][d], distance dist)
{
    int indices_mem[k];
    int y_mem[n];
    int counter = 0;
    indicesmem_init(k, indices_mem);
    for (size_t i = 0; i < k; i++) {
        centroid_init(i, n, d, k, centroids, x, indices_mem);
    }
    do
    {
        copy_cluster_assignment(n, y, y_mem);
        assign_to_centroid(n, d, k, centroids, x, y);
        move_centroid(n, d, k, centroids, x, y);
        if (is_identical_cluster_assignment(n, y, y_mem))
        {
            counter++;
        }else counter = 0;
        
    } while (counter != 2);
    
    
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

    /*
                                                                                              
    "Centroids init --> auf einen Wert von den Datenpunkten && dass die unterschiedlich sind" 

    do
    {
    Datenpunkte den centroids zuordnen -> Abstände von allen Datensätzen Zu den Centroids berechnen --> Der Centroid mit dem geringsten Absstand wird in y von dem Datenbpunkt geschrieben!
    
    Centroid verschieben -> Von jeder Dimension für jeden Centroid einen Mittelwert der zugehörigen Punkte berechnen --> Centroid darauf setzten bzw auf den gerundeten Wert
    --> Frage hat er sich verändert? Irgendwie feststellen z.B vergleichen mit vorherigem Punkt(vlt doch nicht-> kein Zwischenspeicher)
    }while(veränderung der Zugehörigkeit von Datenpunkten zu Centroids zwei mal != 0)
    */

    cluster(n, d, x, y, k, centroids, dist);

    printf("Clustering result:\n");
    for (int i = 0; i < k; i++) {
        printf("Centroid for cluster %d: ", i);
        print_datapoint(d, centroids[i]); //Pointergeschichte noch nachschauen
        printf("\n");
    }
    print_csv(n, d, x, (int*) y);
}
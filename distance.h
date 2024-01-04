#ifndef _DISTANCE_H
#define _DISTANCE_H
typedef enum distance {MANHATTAN, EUCLIDEAN} distance;

double compute_distance(int d, const double x1[d], const double x2[d], distance dist);
#endif
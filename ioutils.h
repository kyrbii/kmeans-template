#ifndef _IOUTILS_H
#define _IOUTILS_H

void print_datapoint(int d, const double x[d]);
void print_csv(int n, int d, const double x[n][d], const int y[n]);
int read_csv(const char* filename, int n, int d, double x[n][d], int y[d]);

// not quite IO-related but let's put it here nonetheless
void copy_instance(int d, const double src[d], double tgt[d]);

#endif
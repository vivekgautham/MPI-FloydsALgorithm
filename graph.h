#ifndef GRAPH_H
#define GRAPH_H
#include <stdio.h>
#include <stdlib.h>
#include "MyMPI.h"
#include <mpi.h>

int min(int a, int b);
void print_graph(int n, int **am);
void read_graph(char *filename, int *n,int ***A);
void write_graph(char *filename, int n, int **A);

#endif

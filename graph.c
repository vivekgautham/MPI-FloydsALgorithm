#include "graph.h"
int min(int a, int b)
{
    if(a > b || a == -1)
        return b;
    return a;
}
void print_graph(int n, int **am)
{
    int i,j;
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            printf("%d ",am[i][j]);
        }
        printf("\n");
    } 
}

void write_graph(char *filename, int n, int **A)
{
    int i;
    FILE* file = fopen(filename,"wb");
    if(!file)
    {
        printf("Cannot Open File for write");
        exit(0);
    }
	fwrite(&n, sizeof(int),1,file);
    for (i = 0;i<n;i++)
    {
    	fwrite(A[i], sizeof(int),n,file);
        
    }
    fclose(file);
}

void read_graph(char *filename, int *n,int ***A)
{
    int i,dim;
    int *numStorage;
    FILE* file = fopen(filename,"rb");
    if(!file)
    {
        printf("Cannot Open File for read");
        exit(0);
    }
    fread(n,sizeof(int),1,file); 
    dim = *n; 
    *A = (int **) malloc(dim*sizeof(int *));
    numStorage = (int *) malloc(dim*dim*sizeof(int));
    for (i=0;i<dim;i++)
    {
        (*A)[i] = &numStorage[i*dim];
	    fread((*A)[i], sizeof(int),dim,file);
    }
    fclose(file);
}



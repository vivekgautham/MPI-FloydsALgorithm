
#include "MyMPI.h"
#include "graph.h"
int main(int argc, char *argv[])
{
    int n,j,k,i=0;
    char *defaultfilename = "default-make-graph-file.dat";
    char *outputfilename = "default-make-graph-file_serial.seq";
    char *filename = NULL;
    int **num;
    double t1,t2,t3,t4;
    int rank,size; 
    MPI_Init (&argc, &argv);
    t1 = MPI_Wtime(); 
    /* get current MPI task number */
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    /* get number of MPI tasks */
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    if(argc > 3)
    {
        printf("You have entered more than 2 options. Enter correct number of argument");
        return 0;
    }

    for (i=1; i<argc; i++)
    {
	    if(strcmp(argv[i],"-i") == 0)
        {
            if(++i < argc)
            { 
                filename = malloc(sizeof(char) * (strlen(argv[i])+1));
                strcpy(filename,argv[i]);
            }
            else
            {
                printf("Incomplete Option");
                return 0;
            }
        } 
    }
    if(!filename)
    {
        filename = malloc(sizeof(char) * (strlen(defaultfilename)+1));
        strcpy(filename,defaultfilename);
    }
/*	
    FILE* file = fopen(filename,"rb");
    if(!file)
    {
        printf("Cannot Open File for read");
        exit(0);
    }
	fread(&n, sizeof(int),1,file);
    numStorage = (int *) malloc(n*n*sizeof(int));
    num = (int **) malloc(n*sizeof(int *));
    for (i=0;i<n;i++)
    {
        num[i] = &numStorage[i*n];
	    fread(num[i], sizeof(int),n,file);
    }
*/
    read_graph(filename,&n,&num);
    
    t2 = MPI_Wtime(); 
    for (k = 0;k<n;k++)
    {
        for (i = 0;i<n;i++)
        {
            for (j = 0;j<n;j++)
            {
                if(i == j)
                    num[i][j] = 0;
                else
                    if(num[i][k] != -1 && num[k][j] != -1)
                        num[i][j] = min(num[i][j],(num[i][k]+num[k][j]));
            }
        }
    }
    
    
    t3 = MPI_Wtime(); 
    //print_graph(n,num);
    write_graph(outputfilename,n,num);
    free(filename);
    t4 = MPI_Wtime(); 
    printf("\n Program Time = %lf \n Floyd's Function Time = %lf \n",(t4-t1),(t3-t2));
    MPI_Finalize();
    return 0;
}

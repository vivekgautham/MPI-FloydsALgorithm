#include "MyMPI.h"
#include <stdbool.h>
#include "graph.h"

void floyd_checkerboard (
   int      ***ab,            /* IN/OUT -2D matrix */
   MPI_Datatype dtype,        /* IN -Matrix element type */
   int          m,            /* IN -Matrix rows */
   int          n,            /* IN -Matrix columns */
   MPI_Comm     grid_comm)    /* IN - Communicator */
{
   int        datum_size;     /* Bytes per matrix element */
   int      *bufferRow;         /* Room to hold 1 matrix row */
   int      *bufferCol;         /* Room to hold 1 matrix Column */

   int        grid_coords[2]; /* Coords of this process */
   int        grid_id;        /* Process rank in grid */
   int        grid_period[2]; /* Wraparound */
   int        grid_size[2];   /* Dims of process grid */

   int        i, j, k;
   int        local_cols;     /* Matrix cols on this proc */
   int        local_rows;     /* Matrix cols on this proc */
   int        p;
//   MPI_Status status;         /* Result of receive */
   int row_sender,col_sender;
   MPI_Comm row_comm;
   MPI_Comm col_comm;
   int **a;
   MPI_Comm_rank (grid_comm, &grid_id);
   MPI_Comm_size (grid_comm, &p);
   datum_size = get_size (dtype);

   MPI_Cart_get (grid_comm, 2, grid_size, grid_period,grid_coords);

   MPI_Comm_split(grid_comm,grid_coords[0],grid_coords[1],&row_comm);
   MPI_Comm_split(grid_comm,grid_coords[1],grid_coords[0],&col_comm);

   local_rows = BLOCK_SIZE(grid_coords[0],grid_size[0],m);
   local_cols = BLOCK_SIZE(grid_coords[1],grid_size[1],n);
   a = *ab;
//   printf("\n Grid ID %d - Rows %d -- Cols %d \n",grid_id,local_rows,local_cols);
   bufferRow = (int *)malloc(local_cols*sizeof(int));
   bufferCol = (int *)malloc(local_rows*sizeof(int));
   for(k=0;k<n;k++)
   {
       row_sender = BLOCK_OWNER(k,grid_size[0],n);
       col_sender = BLOCK_OWNER(k,grid_size[1],n);

       if(grid_coords[0] == row_sender)
       {
            j = k - BLOCK_LOW(grid_coords[0],grid_size[0],n);
            for(i = 0;i<local_cols;i++)
                bufferRow[i] = a[j][i];
    //            printf("\n Iteration k = %d , Row sender (%d,%d) elements %d\n",k,grid_coords[0],grid_coords[1],a[j][i]);
       }
       if(grid_coords[1] == col_sender)
       {
            j = k - BLOCK_LOW(grid_coords[1],grid_size[1],n);
            for(i = 0;i<local_rows;i++)
                bufferCol[i] = a[i][j];
       //         printf("\n Iteration k = %d , Col Sender (%d,%d) elements %d\n",k,grid_coords[0],grid_coords[1],a[i][j]);
       }
       MPI_Bcast(bufferRow,local_cols,MPI_INT,row_sender,col_comm);
       MPI_Bcast(bufferCol,local_rows,MPI_INT,col_sender,row_comm);
       //printf(" %d iteration \n ",k);
        //printf("\n Iteration = %d  Grid ID %d - Row got %d  %d ---- Col Got  %d %d \n",k,grid_id,bufferRow[0],bufferRow[1],bufferCol[0],bufferCol[1]);
        for (i = 0;i<local_rows;i++)
        {
            for (j = 0;j<local_cols;j++)
            {
                 if((i+BLOCK_LOW(grid_coords[0],grid_size[0],n)) == (j+BLOCK_LOW(grid_coords[1],grid_size[1],n)))
                     a[i][j] = 0;
                 else if(bufferRow[j] != -1 && bufferCol[i] != -1)
                     a[i][j] = min(a[i][j],(bufferRow[j] + bufferCol[i]));
            }
        }
        //print_checkerboard_matrix ((void **)a, MPI_INT,n,n,grid_comm);
   }
}
//void fload_checkerboard ( void ***, MPI_Datatype, int, int, MPI_Comm);

int main(int argc, char *argv[])
{
    int i=0;
    char *defaultfilename = "default-make-graph-file.dat";
    char *outputfilename = "default-make-graph-file_parallel.seq";
    char *filename = NULL;
    int **myArray;
    int *myArrayStorage;
    int msub,nsub;
    int rank, size;
    int reorder=0,ndims=2;
    int dims[2],periods[2];
    int coords[2];
    double t1,t2,t3,t4;
    /* run the program with */
    /* never make MPI calls before this and */
    /* never touch argc and argv before doing this */
    MPI_Init (&argc, &argv);
    t1 = MPI_Wtime();
    /* get current MPI task number */
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    /* get number of MPI tasks */
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    MPI_Comm grid_comm;
    //MPI_Comm row_comm;
    //MPI_Comm col_comm;
    dims[0] = dims[1] = 0;
    periods[0] = periods[1] = 0;
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
        else
	    {
	        printf("Invalid Option");
            return 0;
        }

    }
    if(!filename)
    {
        filename = malloc(sizeof(char) * (strlen(defaultfilename)+1));
        strcpy(filename,defaultfilename);
    }

    MPI_Dims_create(size,2,dims);
    MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, reorder, &grid_comm);
    //MPI_Cart_rank(grid_comm, coords, &gridrank);
    MPI_Cart_coords(grid_comm, rank, ndims,coords);

    printf("\n Coords %d %d\n ",coords[0],coords[1]);

    //MPI_Comm_split(grid_comm,coords[0],coords[1],&row_comm);
    //MPI_Comm_split(grid_comm,coords[1],coords[0],&col_comm);

    read_checkerboard_matrix (filename, (void ***)&myArray, (void **)&myArrayStorage , MPI_INT, &msub, &nsub, grid_comm);

    print_checkerboard_matrix ((void **)myArray, MPI_INT,msub ,nsub,grid_comm);
    MPI_Barrier(MPI_COMM_WORLD);
    t2 = MPI_Wtime();
    floyd_checkerboard(&myArray, MPI_INT, msub, nsub, grid_comm);

    MPI_Barrier(MPI_COMM_WORLD);
    t3 = MPI_Wtime();
    //print_checkerboard_matrix ((void **)myArray, MPI_INT,msub ,nsub,grid_comm);
    print_and_write_matrix ((void **)myArray,outputfilename, MPI_INT,msub ,nsub,grid_comm);


    free(filename);
    t4 = MPI_Wtime();
    MPI_Finalize();
    printf("\n Program Time = %lf \n Floyd's Function Time = %lf \n",(t4-t1),(t3-t2));
    return 0;
}

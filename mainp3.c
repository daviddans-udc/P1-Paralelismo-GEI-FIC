#include <stdio.h>
#include <sys/time.h>
#include <mpi.h>
#include <math.h>

#define DEBUG 1
 
#define N 1024

int main(int argc, char *argv[] ) {

    int i, j;
    float matrix[N][N];
    float vector[N];
    float result[N];
    struct timeval  tv1, tv2;
    int numprocs, rank, msize;

      MPI_Init(&argc,&argv); //mpi init
      MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
      MPI_Comm_rank( MPI_COMM_WORLD, &rank     );

    if(rank == 0){
        /* Initialize Matrix and Vector */
        for(i=0;i<N;i++) {
            vector[i] = i;
            for(j=0;j<N;j++) {
                matrix[i][j] = i+j;
            }
        }
    }

    msize = ceil(N/numprocs);

    float mbuff[msize][N];
    float rbuff[msize];
    //Scater
    MPI_Scatter(matrix,N*msize,MPI_FLOAT,mbuff,N*msize,MPI_FLOAT,0,MPI_COMM_WORLD);
    MPI_Bcast(vector,N,MPI_FLOAT,0,MPI_COMM_WORLD);
    //gettimeofday(&tv1, NULL);

    /*Carga a paralelizar*/
    for(i=0;i<msize;i++){
        rbuff[i]=0;
        for(j=0;j<N;j++){
            //printf("matrix [%d][%d] : %f",i, j, mbuff[i][j]);
            rbuff[i] += mbuff[i][j]*vector[j];
        }
    }

    //Gather
    MPI_Gather(rbuff,msize,MPI_FLOAT,result,msize,MPI_FLOAT,0,MPI_COMM_WORLD);

    //gettimeofday(&tv2, NULL);
    
    //int microseconds = (tv2.tv_usec - tv1.tv_usec)+ 1000000 * (tv2.tv_sec - tv1.tv_sec);

    /*Display result */
    if(rank == 0){
        if (DEBUG){
            for(i=0;i<N;i++) {
            printf(" %f \t ",result[i]);
            }
        }else{
            //printf ("Time (seconds) = %lf\n", (double) microseconds/1E6);
        }    
    }
    MPI_Finalize(); //mpi finalize
    return 0;
}


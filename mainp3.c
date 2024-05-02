#include <stdio.h>
#include <sys/time.h>
#include <mpi.h>
#include <math.h>

#define DEBUG 0
 
#define N 50000

int main(int argc, char *argv[] ) {

    int i, j;
    float vector[N];
    struct timeval  tcomp1, tcomp2, tglob1, tglob2;
    int numprocs, rank, msize, bloque;

    MPI_Init(&argc,&argv); //mpi init
    MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank     );
 
    msize = ceil((float)N/(float)numprocs);

    float matrix[numprocs*msize][N];
    float result[numprocs*msize];
    float mbuff[msize][N];
    float rbuff[msize];
    if(rank < numprocs -1) bloque = msize;
    else bloque = msize - (N % numprocs);

    if(rank == 0){
        /* Initialize Matrix and Vector */
        for(i=0;i<N;i++) {
            vector[i] = i;
            for(j=0;j<N;j++) {
                matrix[i][j] = i+j;
            }
        }
    }

    gettimeofday(&tglob1, NULL); //Timestapm global

    //Scater matrix
    if(MPI_SUCCESS != MPI_Scatter(matrix,N*msize,MPI_FLOAT,mbuff,N*msize,MPI_FLOAT,0,MPI_COMM_WORLD)) return -1;
    //Broadcast vector
    if(MPI_SUCCESS != MPI_Bcast(vector,N,MPI_FLOAT,0,MPI_COMM_WORLD)) return -1;


    gettimeofday(&tcomp1, NULL); //Timestapm compute
    /*Carga a paralelizar*/
    
    for(i=0;i<bloque;i++){
        rbuff[i]=0;
        for(j=0;j<N;j++){
            //printf("matrix [%d][%d] : %f",i, j, mbuff[i][j]);
            rbuff[i] += mbuff[i][j]*vector[j];
        }
    }
    gettimeofday(&tcomp2, NULL); //Timestamp compute

    int localComputeTime = (tcomp2.tv_usec - tcomp1.tv_usec) + 1000000 * (tcomp2.tv_sec - tcomp2.tv_sec); //Tiempo de computo local

    //Gather result
    if(MPI_SUCCESS != MPI_Gather(rbuff,msize,MPI_FLOAT,result,msize,MPI_FLOAT,0,MPI_COMM_WORLD)) return -1;

    gettimeofday(&tglob2, NULL); //Timestapm global

    int localComsTime = ( (tglob2.tv_usec - tglob1.tv_usec) + 1000000 * (tglob2.tv_sec - tglob2.tv_sec) ) - localComputeTime; //Tiempo de comunicación local
    
    char message[75];
    char messageS[numprocs][75];
    sprintf(message,"PROCESS: %d -> Compute Time = %lf | Comunications Time = %lf", rank, (double) localComputeTime/1E6, (double) localComsTime/1E6);

    MPI_Gather(message,75,MPI_CHAR,messageS,75,MPI_CHAR,0,MPI_COMM_WORLD);

    /*Display result */
    if(rank == 0){
        if (DEBUG){
            printf("Vector result: \n");
            for(i=0;i<N;i++) {
            printf(" result[%d]:\t %f \t ",i,result[i]);
            }
        }else{
            //printf ("PROCESS: %d -> Compute Time = %lf | Comunications Time = %lf\n", rank, (double) localComputeTime/1E6, (double) localComsTime/1E6);
            for(i = 0; i < numprocs; i++){
                printf("%s\n",messageS[i]);
            }
        }    
    }
    MPI_Finalize(); //mpi finalize
    return 0;
}


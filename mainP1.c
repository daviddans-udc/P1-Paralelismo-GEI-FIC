#include <stdio.h>
#include <math.h>
#include <mpi.h>


int main(int argc, char *argv[])
{
    int i, done = 0, n, err;
    double PI25DT = 3.141592653589793238462643;
    double pi, h, sum, x;
    //mpi variables
    int numprocs;
    int rank;
    double buff;

    MPI_Init(&argc,&argv); //mpi init
    MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank     );

    while (!done){
    
        // distribuir n
        if(rank == 0){
            printf("Enter the number of intervals: (0 quits) \n");
            scanf("%d",&n);
            for(i = 1;i<numprocs;i++){
                if((err = MPI_Send(&n,1,MPI_INT,i,1,MPI_COMM_WORLD)) != MPI_SUCCESS) MPI_Abort(MPI_COMM_WORLD,err);
            }
           }
        else{
            if((err = MPI_Recv(&n,1,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,NULL)) != MPI_SUCCESS) MPI_Abort(MPI_COMM_WORLD,err);
        }
        h   = 1.0 / (double) n;
        sum = 0.0;
        
        if (n == 0) break;

        //Carga a paralelizar
        for (i=rank+1; i<=n; i+=numprocs){
            x = h * ((double)i - 0.5);
            sum += 4.0 / (1.0 + x*x);
        }

        //recive & send
        if(rank == 0){
            for(i = 0;i<numprocs-1;i++){
            if((err = MPI_Recv(&buff,1,MPI_DOUBLE,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,NULL)) != MPI_SUCCESS) MPI_Abort(MPI_COMM_WORLD,err);
            sum += buff; //Sumar pi
            }
            //Calcular e imprimir pi
            pi = h*sum;
             printf("pi is approximately %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
        }
        else{
            if((err = MPI_Send(&sum,1,MPI_DOUBLE,0,1,MPI_COMM_WORLD)) != MPI_SUCCESS) MPI_Abort(MPI_COMM_WORLD,err);
        }
    }
    MPI_Finalize(); //mpi finalize
    return 0;
}
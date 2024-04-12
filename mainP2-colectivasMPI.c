#include <stdio.h>
#include <math.h>
#include <mpi.h>


int main(int argc, char *argv[])
{
    int i, done = 0, n;
    double PI25DT = 3.141592653589793238462643;
    double pi, h, sum, sumTot, x;
    //mpi variables
    int numprocs;
    int rank;

    MPI_Init(&argc,&argv); //mpi init
    MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank     );

    while (!done){
    
        // distribuir n Con colectiva mpi
        if(rank == 0){
            printf("Enter the number of intervals: (0 quits) \n");
            scanf("%d",&n);
           }
        MPI_Bcast(&n,1,MPI_INTEGER,0,MPI_COMM_WORLD); //Bcast
        h   = 1.0 / (double) n;
        sum = 0.0;
        
        if (n == 0) break;

        //Carga a paralelizar
        for (i=rank+1; i<=n; i+=numprocs){
            x = h * ((double)i - 0.5);
            sum += 4.0 / (1.0 + x*x);
        }

        //recive & send Con colectiva mpi
        MPI_Reduce(&sum,&sumTot,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);

        if(rank == 0){
            //Calcular e imprimir pi
            pi = h*sumTot;
             printf("pi is approximately %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
        }
    }
    MPI_Finalize(); //mpi finalize
}
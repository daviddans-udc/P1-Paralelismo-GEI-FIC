#include <stdio.h>
#include <math.h>
#include <mpi.h>

//Colectiva reduce propia
int MPI_FlattreeColectiva(void* buff, void* recvbuff, int count,
                        MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm){
    int rank, numprocs, i;
    if(op != MPI_SUM) return MPI_ERR_OP; // control de errores
    if(datatype != MPI_DOUBLE){return MPI_ERR_TYPE;}
    
    //obtener size y rank 
    MPI_Comm_size( comm, &numprocs ); 
    MPI_Comm_rank( comm, &rank  );
    //Comunicación
    if(rank == root){
            *(double*)recvbuff = *(double*)buff;
            for(i = 0;i<numprocs-1;i++){
            MPI_Recv(buff,1,MPI_DOUBLE,MPI_ANY_SOURCE,MPI_ANY_TAG,comm,NULL);
            *(double*)recvbuff += *(double*)buff; //Sumar trozos
            }
        }
        else{
            MPI_Send(buff,1,MPI_DOUBLE,0,1,comm);
        }
    return MPI_SUCCESS;
}

//Colectiva bcast propia
int MPI_BinomialBcast(void* buff, int count, MPI_Datatype datatype, int root, MPI_Comm comm){
    int rank, numprocs, i;
    if(datatype != MPI_INTEGER) return MPI_ERR_TYPE; //control de error
    
    //obtener size y rank 
    MPI_Comm_size( comm, &numprocs ); 
    MPI_Comm_rank( comm, &rank  );
    
    //comunicacion
    if(rank != root){
        MPI_Recv(buff,count,datatype,MPI_ANY_SOURCE,1,comm,NULL);
    }
    for(i = 1; rank+i<numprocs; i<<=1){
        if(rank < i){
            MPI_Send(buff,count,datatype,rank+i,1,comm);
        }
    }
    return MPI_SUCCESS;
}


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
    
        // distribuir n Con colectiva propia
        if(rank == 0){
            printf("Enter the number of intervals: (0 quits) \n");
            scanf("%d",&n);
           }
        MPI_BinomialBcast(&n,1,MPI_INTEGER,0,MPI_COMM_WORLD);
        h   = 1.0 / (double) n;
        sum = 0.0;
        
        if (n == 0) break;

        //Carga a paralelizar
        for (i=rank+1; i<=n; i+=numprocs){
            x = h * ((double)i - 0.5);
            sum += 4.0 / (1.0 + x*x);
        }

        //recive & send Con colectiva propia
        MPI_Reduce(&sum,&sumTot,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);

        if(rank == 0){
            //Calcular e imprimir pi
            pi = h*sumTot;
            printf("pi is approximately %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
        }
    }
    MPI_Finalize(); //mpi finalize
}
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAG 13

int main(int argc, char *argv[]) {
    double **A, *B, *X, *tmp, sum=0.0;
    double startTime, endTime;
    int numElements, offset, stripSize, myrank, numnodes, N, i, j, k;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &numnodes);
    
    N = atoi(argv[1]);
    int *workMap, *inttmp;
    
    // allocate A, B, workMap, and X --- note that you want these to be
    // contiguously allocated.  Workers need less memory allocated.

    tmp = (double *) malloc (sizeof(double ) * N * N);
    A = (double **) malloc (sizeof(double *) * N);
    for (i = 0; i < N; i++)
        A[i] = &tmp[i * N];
    
    tmp = (double *) malloc (sizeof(double ));
    B = (double *) malloc (sizeof(double *) * N);
    for (i = 0; i < N; i++)
        B[i] = tmp[i];

    inttmp = (int *) malloc (sizeof(int ));
    workMap = (int *) malloc (sizeof(int *) * N);
    for (i = 0; i < N; i++)
        workMap[i] = inttmp[i];

    if (myrank == 0) {
        tmp = (double *) malloc (sizeof(double ));
        X = (double *) malloc (sizeof(double *) * N);
        for (i = 0; i < N; i++)
            X[i] = tmp[i];
            X[i] = 0.0;
    }
    
    //Fill A and B with random data
    if (myrank == 0) {
        srand(time(NULL));
        for (i=0; i<N; i++) {
            for (j=0; j<N; j++) {
                A[i][j] = (float)rand() / 32768.0;
            }
            B[i] = (float)rand() / 32768.0;
        }
    }

    //Print inputs if N<10
    if (myrank == 0 && N < 10) {
        printf("\nA:\n");
        for (i=0; i<N; i++) {
            for(j=0; j<N; j++){
                printf("%f ", A[i][j]);
            }
            printf("\n");
        }
        printf("\nB:\n");
        for (i=0; i<N; i++) {
            printf("%f ", B[i]);
        }
        printf("\n\n");
    }

    //Make sure work can be evenly handed out
    stripSize = N/numnodes;
    //if stripSize is invalid yell and quit
    if(N % numnodes != 0){
        MPI_Finalize();
        if(myrank == 0){
            printf("Input size is not divisible by No of nodes!\n");
        }
        exit(-1);
    }
    else{
        //if stripSize is valid print it out
        if(myrank == 0){
            printf("stripSize: %d\n",stripSize);
        }
    }

    // start timer
    if (myrank == 0) {
        startTime = MPI_Wtime();
    }

    // everyone gets A -- note could be done via MPI_Bcast
    if (myrank == 0) {
        printf("Starting initital communication.\n");
        numElements = N * N;
        for (i=1; i<numnodes; i++) {
            MPI_Send(&(A[0][0]), numElements, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD);
        }
    }
    else{   // receive A
        MPI_Recv(&(A[0][0]), N * N, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    // everyone gets B -- note could be done via MPI_Bcast
    if (myrank == 0){
        numElements = N;
        for (i=1; i<numnodes; i++) {
            MPI_Send(&B[0], numElements, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD);
        }
    }
    else{   // receive B
        MPI_Recv(&B[0], N, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Divide the work
    if(myrank == 0){
        for(i = 0; i < N; i++){
            workMap[i] = i % numnodes;
        }
        printf("Made workMap.\n");
    }

    // send everyone workMap -- note can be done with MPI_Bcast
    if (myrank == 0){
        numElements = N;
        for (i=1; i<numnodes; i++) {
            MPI_Send(&workMap[0], numElements, MPI_INT, i, TAG, MPI_COMM_WORLD);
        }
    }
    else{   //recieve workMap
        MPI_Recv(&workMap[0], N, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    // Announce end of communication
    if(myrank == 0){
        printf("Done with initital communication.\nStarting work.\n");
    }

    // do the work
    double mult = 0.0;
    for(k = 0; k < N-1; k++){
        //if(myrank == 0){
        //    printf("Iteration %d\n",k);
        //}
        //printf("%d : k= %d, workMap[k]= %d\n",myrank,k,workMap[k]);
        for(i = k+1; i < N; i++){
            if(workMap[i] == myrank){
                mult = A[i][k]/A[k][k];
                for(j = 0; j < N; j++){
                    A[i][j] = A[i][j] - mult*A[k][j];
                }
                B[i] = B[i]- mult*B[k];
            }
        }
        MPI_Bcast(&(A[k][k]), N-k, MPI_DOUBLE, workMap[k], MPI_COMM_WORLD);
        MPI_Bcast(&B[k], 1, MPI_DOUBLE, workMap[k], MPI_COMM_WORLD);
    }

    // only share final work if running in parallel
    if(numnodes>1){
        // Synchronize at end of work so we know final worker has completed loop
        MPI_Barrier(MPI_COMM_WORLD);

        // master recieves final worker's A
        if(myrank==0){
            printf("Starting final communication.\n");
            numElements = N * N;
            MPI_Recv(&(A[0][0]), numElements, MPI_DOUBLE, numnodes-1, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        }
        else if(myrank==numnodes-1){
            MPI_Send(&(A[0][0]), N * N, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
        }

        // master recieves final worker's B
        if(myrank==0){
            numElements = N;
            MPI_Recv(&B[0], numElements, MPI_DOUBLE, numnodes-1, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        }
        else if(myrank==numnodes-1){
            MPI_Send(&B[0], N, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
        }

        // announce end of communication
        if(myrank == 0){
            printf("Done with final communication.\n");
        }
    }

    // master does back prop
    if(myrank == 0){
        printf("Starting back prop.\n");
        X[N-1]=B[N-1]/A[N-1][N-1];
        for(i = N-2; i >= 0; i--){
            sum=0.0;
            for(j = i+1; j < N; j++){
                sum+=A[i][j]*X[j];
            }
            X[i] = (B[i]-sum)/A[i][i];
        }
    }

    // stop timer
    if (myrank == 0) {
        endTime = MPI_Wtime();
        printf("Execution Time is %f (seconds)\n", endTime-startTime);
    }
    
    // print output matrix here, if I'm the master
    if (myrank == 0 && N < 10) {
        printf("\nX:\n");
        for (i=0; i<N; i++) {
            printf("%f ", X[i]);
        }
        printf("\n\n");
    }


    MPI_Finalize();
    return 0;
}
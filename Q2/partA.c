#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAG 13

int main(int argc, char *argv[]) {
    double **A, *B, *X, *tmp;
    double startTime, endTime;
    int numElements, offset, stripSize, myrank, numnodes, N, i, j, k;
    
    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &numnodes);
    
    N = atoi(argv[1]);

    // allocate A, B, and C --- note that you want these to be
    // contiguously allocated.  Workers need less memory allocated.

    if (myrank == 0) {
        tmp = (double *) malloc (sizeof(double ) * N * N);
        A = (double **) malloc (sizeof(double *) * N);
        for (i = 0; i < N; i++)
            A[i] = &tmp[i * N];
    }
    else {
        tmp = (double *) malloc (sizeof(double ) * N * N / numnodes);
        A = (double **) malloc (sizeof(double *) * N / numnodes);
        for (i = 0; i < N / numnodes; i++)
            A[i] = &tmp[i * N];
    }
    
    
    tmp = (double *) malloc (sizeof(double ) * N);
    B = (double *) malloc (sizeof(double ) * N);
    for (i = 0; i < N; i++)
        B[i] = &tmp[i];
    
    
    if (myrank == 0) {
        tmp = (double *) malloc (sizeof(double ) * N);
        X = (double *) malloc (sizeof(double ) * N);
        for (i = 0; i < N; i++)
            X[i] = &tmp[i];
            X[i] = 0.0;
    }
    //else {
    //    tmp = (double *) malloc (sizeof(double ) * N / numnodes);
    //    X = (double *) malloc (sizeof(double ) * N / numnodes);
    //    for (i = 0; i < N / numnodes; i++)
    //        X[i] = &tmp[i];
    //}
    
    if (myrank == 0) {
        srand(time(NULL));
        // initialize A and B
        for (i=0; i<N; i++) {
            for (j=0; j<N; j++) {
                A[i][j] = (float)rand() / 32768.0;
            }
            B[i] = (float)rand() / 32768.0;
        }
    }
    
    // start timer
    if (myrank == 0) {
        startTime = MPI_Wtime();
    }
    
    stripSize = N/numnodes;

    // send each node its piece of A -- note could be done via MPI_Scatter
    if (myrank == 0) {
        offset = stripSize;
        numElements = stripSize * N;
        for (i=1; i<numnodes; i++) {
            MPI_Send(A[offset], numElements, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD);
            offset += stripSize;
        }
    }
    else {  // receive my part of A
        MPI_Recv(A[0], stripSize * N, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    // everyone gets B
    //MPI_Bcast(B[0], N*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    if (myrank == 0){
        offset = stripSize;
        numElements = stripSize;
        for (i=1; i<numnodes; i++) {
            MPI_Send(B[offset], numElements, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD);
            offset += stripSize;
        }
    }
    else{
        MPI_Recv(B[0], stripSize, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Let each process initialize X to zero 
    //for (i=0; i<stripSize; i++) {
    //    X[i] = 0.0;
    //}

    // do the work
    for (k = 0; k<stripSize; k++) {
        for (i = k+1; i<N; i++){
            A[i][k] /= A[k][k];
            B[i] -= A[i][k]*B[k];
        }
        for (i = k+1; i<N; i++) {
            for (j = k+1; j<N; j++) {
            	A[i][j] -= A[i][k] * A[k][j];
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // master receives A from workers  -- note could be done via MPI_Gather
    if (myrank == 0) {
        offset = stripSize; 
        numElements = stripSize * N;
        for (i=1; i<numnodes; i++) {
            MPI_Recv(A[offset], numElements, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            offset += stripSize;
        }
    }
    else { // send my contribution to A
        MPI_Send(A[0], stripSize * N, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
    }
    //Send B
    if (myrank == 0){
        offset = stripSize; 
        numElements = stripSize;
        for (i=1; i<numnodes; i++) {
            MPI_Recv(B[offset], numElements, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            offset += stripSize;
        }
    }
    else{
        MPI_Send(B[0], stripSize, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
    }

    if(myrank == 0){
        for(i = N-1; i >= 0; i--){
            X[i] = B[i];
            for(j = i+1; j < N; j++){
                X[i] -= A[i][j] * X[j];
            }
            X[i] /= A[i][i];
        }
    }

    // stop timer
    if (myrank == 0) {
        endTime = MPI_Wtime();
        printf("Execution Time is %f (seconds)\n", endTime-startTime);
    }
    
    // print out matrix here, if I'm the master
    if (myrank == 0 && N < 10) {
        for (i=0; i<N; i++) {
            printf("%f ", X[i]);
        }
        printf("\n");
    }
    
    MPI_Finalize();
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


long long cmp_fact(int n){
	long long fact = 1;
	for(int i = 1; i <= n; i++){
		fact *= i;
	}
	return fact;
	}
	

int main(int argc, char* argv[]){
	int rank, size;
	int *values = NULL;
	int rec_val;
	long long factRes;
	
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	
	if (rank == 0){
		values = (int *)malloc(size*sizeof(int));
		printf("Enter the %d integer values:\n", size);
		for(int i = 0; i < size; i++){
			scanf("%d", &values[i]);
		}
	}
	
	
	MPI_Scatter(values, 1, MPI_INT, &rec_val, 1, MPI_INT, 0,MPI_COMM_WORLD);
	
	factRes = cmp_fact(rec_val);
	
	printf("Process %d received %d and compute factorial: %lld\n",rank, rec_val, factRes);

	if(rank == 0){
		free(values);
		
	}
	MPI_Finalize();
	return 0;
}


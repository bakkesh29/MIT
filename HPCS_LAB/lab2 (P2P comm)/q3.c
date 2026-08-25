#include<mpi.h>
#include<stdio.h>


int main(int argc, char* argv[]){
	int rank,size;
	int N = 10;
	int arr[10] = {1,2,3,4,5,6,7,8,9,10};
	int local[5];
	int local_sum = 0;
	int total_sum =0;
	
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	if (size != 2){
		if (rank == 0)
			printf("Run using 2 processes\n");
			MPI_Finalize();
			return 0;
			}
	MPI_Scatter(arr,N/2,MPI_INT,local,N/2,MPI_INT,0,MPI_COMM_WORLD);
	
	for(int i = 0; i < N; i++){
		local_sum += local[i];
		}
	printf("Process %d local sum =  %d\n", rank, local_sum);
	
	MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
	
	if (rank == 0){
		printf("Total sum = %d\n",total_sum);
		}
	MPI_Finalize();
	return 0;
	}
		

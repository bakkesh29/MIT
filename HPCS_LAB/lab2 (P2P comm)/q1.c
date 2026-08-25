#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>


int main(int argc, char* argv[]){
	int rank, size, a,b;
	double res;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	
	a = atoi(argv[0]);
	b = atoi(argv[1]);
	
	if(rank == 0){
		printf("Calculator: Each process uses its rank to pick an operation \n");
		fflush(stdout);
		}
	switch(rank){
		case 0:
			res = a + b;
			printf("Process %d: sum of %d & %d is %.2f\n", rank, a, b,res);
			break;
		case 1:
			res = a - b;
			printf("Process %d: difference of %d & %d is %.2f\n",rank, a, b,res);
			break;
		case 2:
			res = a * b;
			printf("Process %d: product of %d & %d is %.2f\n",rank, a, b,res);
			break;
		case 3:
			if(b != 0){
				res = (double)a/b;
				printf("Process %d: quotient of %d & %d is %.2f\n", rank,a,b, res);
				}
			else{
				printf("Process %d: division by zero error\n",rank);
				}
			break;
		default:
			printf("Process %d: no operation assigned \n", rank);
			}
	fflush(stdout);
	MPI_Finalize();
	return 0;
	}

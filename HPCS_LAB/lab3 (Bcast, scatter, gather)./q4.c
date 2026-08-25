#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int rank, size;
    int M = 0;
    int *global_array = NULL;
    int *local_array = NULL;
    int *result_array = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // N is implicitly defined by the number of running processes (size)
    int N = size; 

    // Step 1: Root process reads M and the array elements
    if (rank == 0) {
        printf("Enter the value of M (number of elements per process): ");
        fflush(stdout);
        if (scanf("%d", &M) != 1) {
            printf("Invalid input.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Allocate memory for NxM elements
        global_array = (int*)malloc(N * M * sizeof(int));
        result_array = (int*)malloc(N * M * sizeof(int));

        printf("Enter %d elements:\n", N * M);
        fflush(stdout);
        for (int i = 0; i < N * M; i++) {
            if (scanf("%d", &global_array[i]) != 1) {
                printf("Invalid input.\n");
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
    }

    // Step 2: Broadcast the value of M to all processes
    MPI_Bcast(&M, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Allocate local memory for each process to hold M elements
    local_array = (int*)malloc(M * sizeof(int));

    // Step 3: Scatter the elements so each process gets M elements
    MPI_Scatter(global_array, M, MPI_INT, local_array, M, MPI_INT, 0, MPI_COMM_WORLD);

    // Step 4: Perform operations based on the process rank
    // Even ranks (0, 2, 4...) square the numbers
    // Odd ranks (1, 3, 5...) cube the numbers
    if (rank % 2 == 0) {
        for (int i = 0; i < M; i++) {
            local_array[i] = local_array[i] * local_array[i];
        }
    } else {
        for (int i = 0; i < M; i++) {
            local_array[i] = local_array[i] * local_array[i] * local_array[i];
        }
    }

    // Step 5: Gather the processed elements back to the root
    MPI_Gather(local_array, M, MPI_INT, result_array, M, MPI_INT, 0, MPI_COMM_WORLD);

    // Step 6: Root process prints the final result
    if (rank == 0) {
        printf("\nThe final result array in root is:\n");
        for (int i = 0; i < N * M; i++) {
            printf("%d ", result_array[i]);
        }
        printf("\n");

        free(global_array);
        free(result_array);
    }

    free(local_array);
    MPI_Finalize();
    return 0;
}


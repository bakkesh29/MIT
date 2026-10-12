/*
 * HPCS Lab - Week 3: MPI Collectives
 * Program 1: Factorial Calculation Using MPI_Scatter
 *
 * Description:
 *   Rank 0 reads an array of integers—one integer for each active process.
 *   MPI_Scatter is used to distribute one integer to each process.
 *   Each process calculates the factorial of its received integer and prints
 *   the result.
 *
 * Process Count & Collective Mapping:
 *   - The number of input values entered at Rank 0 MUST match the number
 *     of running processes (size) because MPI_Scatter sends exactly 1 element
 *     to each process in the communicator.
 *
 * Execution:
 *   mpirun -np 4 ./factorial_scatter
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Function to compute factorial
long long computeFactorial(int n) {
    if (n <= 1) return 1;
    long long fact = 1;
    for (int i = 2; i <= n; i++) {
        fact *= i;
    }
    return fact;
}

int main(int argc, char *argv[]) {
    int rank, size;
    int local_val = 0;
    int *input_arr = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("============================================================\n");
        printf("   MPI Program 1: Factorial Distribution via MPI_Scatter    \n");
        printf("============================================================\n");
        printf("Total active processes = %d\n", size);
        printf("Note: Exactly %d integers must be entered (1 per process).\n\n", size);

        input_arr = (int *)malloc(size * sizeof(int));
        if (input_arr == NULL) {
            printf("Memory allocation failed!\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        printf("Enter %d integers (non-negative, <= 20 to prevent overflow):\n", size);
        for (int i = 0; i < size; i++) {
            scanf("%d", &input_arr[i]);
            if (input_arr[i] < 0) {
                printf("Warning: Negative integer entered at index %d; setting to 0.\n", i);
                input_arr[i] = 0;
            }
        }
    }

    // Collective Scatter: Rank 0 sends 1 integer to every process
    MPI_Scatter(input_arr, 1, MPI_INT,
                &local_val, 1, MPI_INT,
                0, MPI_COMM_WORLD);

    // Each process calculates factorial of its received number
    long long local_fact = computeFactorial(local_val);

    // Each process prints its computation
    printf("[Process %d of %d] Received integer: %d -> Factorial: %lld\n",
           rank, size, local_val, local_fact);

    if (rank == 0) {
        free(input_arr);
    }

    MPI_Finalize();
    return 0;
}

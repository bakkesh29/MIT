/*
 * HPCS Lab - Week 3: MPI Collectives
 * Program 4: Grouped Powers Using MPI_Scatter and MPI_Gather
 *
 * Description:
 *   Rank 0 reads N groups of M integers and scatters one group to each process.
 *   Each process raises the integers in its group to a specific power:
 *     - Process 0: squares its elements     (power 2 = rank + 2)
 *     - Process 1: cubes its elements       (power 3 = rank + 2)
 *     - Process 2: raises to power 4        (power 4 = rank + 2)
 *     - ...
 *     - Process k: raises to power (k + 2)
 *   Powers are calculated using a loop (no pow() function).
 *   The results are gathered at Rank 0 using MPI_Gather and displayed.
 *
 * Execution:
 *   mpirun -np 3 ./grouped_powers
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Function to compute integer power using a loop without pow()
long long computePower(int base, int exp) {
    long long result = 1;
    for (int i = 0; i < exp; i++) {
        result *= base;
    }
    return result;
}

int main(int argc, char *argv[]) {
    int rank, size;
    int M = 0;
    int *all_data = NULL;
    long long *gathered_results = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        int N;
        printf("============================================================\n");
        printf("   MPI Program 4: Grouped Powers (MPI_Scatter & Gather)     \n");
        printf("============================================================\n");
        printf("Total active processes = %d\n", size);
        printf("Enter number of groups (N, must equal %d): ", size);
        if (scanf("%d", &N) != 1 || N != size) {
            printf("Error: Number of groups N (%d) must match process count (%d)!\n", N, size);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        printf("Enter number of elements per group (M): ");
        if (scanf("%d", &M) != 1 || M <= 0) {
            printf("Invalid group size M!\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        all_data = (int *)malloc(size * M * sizeof(int));
        gathered_results = (long long *)malloc(size * M * sizeof(long long));

        if (all_data == NULL || gathered_results == NULL) {
            printf("Host memory allocation failed!\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        printf("Enter %d integers (small positive integers recommended):\n", size * M);
        for (int i = 0; i < size; i++) {
            printf("  Group %d (%d integers): ", i, M);
            for (int j = 0; j < M; j++) {
                scanf("%d", &all_data[i * M + j]);
            }
        }
    }

    // 1. Broadcast group size M to all processes
    MPI_Bcast(&M, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // 2. Allocate local buffers
    int *local_data = (int *)malloc(M * sizeof(int));
    long long *local_res = (long long *)malloc(M * sizeof(long long));

    if (local_data == NULL || local_res == NULL) {
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // 3. Scatter one group of M integers to each process
    MPI_Scatter(all_data, M, MPI_INT,
                local_data, M, MPI_INT,
                0, MPI_COMM_WORLD);

    // 4. Compute power (rank + 2) using a loop
    int exponent = rank + 2;
    for (int i = 0; i < M; i++) {
        local_res[i] = computePower(local_data[i], exponent);
    }

    // 5. Gather results back at Rank 0
    MPI_Gather(local_res, M, MPI_LONG_LONG,
               gathered_results, M, MPI_LONG_LONG,
               0, MPI_COMM_WORLD);

    // 6. Display results at Rank 0
    if (rank == 0) {
        printf("\n--- Computed Group Powers Gathered at Rank 0 ---\n");
        for (int i = 0; i < size; i++) {
            int exp = i + 2;
            printf("Group %d (Processed by Rank %d, Power ^%d):\n  ", i, i, exp);
            for (int j = 0; j < M; j++) {
                int orig = all_data[i * M + j];
                long long computed = gathered_results[i * M + j];
                printf("%d^%d = %lld   ", orig, exp, computed);
            }
            printf("\n");
        }

        free(all_data);
        free(gathered_results);
    }

    free(local_data);
    free(local_res);

    MPI_Finalize();
    return 0;
}

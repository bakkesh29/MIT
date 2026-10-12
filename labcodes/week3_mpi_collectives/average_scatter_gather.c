/*
 * HPCS Lab - Week 3: MPI Collectives
 * Program 2: Group Averages Using MPI_Scatter and MPI_Gather
 *
 * Description:
 *   Rank 0 reads N groups of M floating-point numbers.
 *   Rank 0 scatters one group of M numbers to each process using MPI_Scatter.
 *   Each process calculates the average of its group of M numbers.
 *   The group averages are gathered at Rank 0 using MPI_Gather.
 *   Rank 0 prints the individual group averages and calculates the overall average.
 *
 * Process Requirement:
 *   The number of groups (N) must equal the total number of MPI processes (size).
 *
 * Execution:
 *   mpirun -np 3 ./average_scatter_gather
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int M = 0;
    float *all_data = NULL;
    float *group_avgs = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        int N;
        printf("============================================================\n");
        printf("   MPI Program 2: Group Averages (MPI_Scatter & MPI_Gather) \n");
        printf("============================================================\n");
        printf("Total active MPI processes = %d\n", size);
        printf("Enter number of groups (N, must equal process count %d): ", size);
        if (scanf("%d", &N) != 1 || N != size) {
            printf("Error: Number of groups N (%d) must match process count (%d)!\n", N, size);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        printf("Enter number of elements per group (M): ");
        if (scanf("%d", &M) != 1 || M <= 0) {
            printf("Invalid value for M!\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        all_data = (float *)malloc(size * M * sizeof(float));
        group_avgs = (float *)malloc(size * sizeof(float));

        if (all_data == NULL || group_avgs == NULL) {
            printf("Host memory allocation failed!\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        printf("Enter %d numbers (%d groups of %d):\n", size * M, size, M);
        for (int i = 0; i < size; i++) {
            printf("  Group %d (%d numbers): ", i, M);
            for (int j = 0; j < M; j++) {
                scanf("%f", &all_data[i * M + j]);
            }
        }
    }

    // 1. Broadcast group size M to all processes
    MPI_Bcast(&M, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // 2. Allocate buffer for local group
    float *local_data = (float *)malloc(M * sizeof(float));
    if (local_data == NULL) {
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // 3. Scatter one group of M floats to each process
    MPI_Scatter(all_data, M, MPI_FLOAT,
                local_data, M, MPI_FLOAT,
                0, MPI_COMM_WORLD);

    // 4. Each process computes its group average
    float local_sum = 0.0f;
    for (int i = 0; i < M; i++) {
        local_sum += local_data[i];
    }
    float local_avg = local_sum / (float)M;

    // 5. Gather all local averages back at Rank 0
    MPI_Gather(&local_avg, 1, MPI_FLOAT,
               group_avgs, 1, MPI_FLOAT,
               0, MPI_COMM_WORLD);

    // 6. Rank 0 displays results and overall average
    if (rank == 0) {
        printf("\n--- Gathered Group Averages at Rank 0 ---\n");
        float total_avg_sum = 0.0f;
        for (int i = 0; i < size; i++) {
            printf("Group %d (Processed by Rank %d) Average = %.2f\n",
                   i, i, group_avgs[i]);
            total_avg_sum += group_avgs[i];
        }

        float overall_avg = total_avg_sum / (float)size;
        printf("\nOverall Average of all %d numbers = %.2f\n", size * M, overall_avg);

        free(all_data);
        free(group_avgs);
    }

    free(local_data);
    MPI_Finalize();
    return 0;
}

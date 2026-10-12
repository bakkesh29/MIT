/*
 * HPCS Lab - Week 4: MPI Reduce and Scan
 * Program 3: Column-Wise Prefix Sum Across Rows Using MPI_Scan (4x4 Matrix)
 *
 * Description:
 *   Rank 0 reads a 4x4 matrix.
 *   Using 4 processes, each process receives one row (4 integers) via MPI_Scatter.
 *   MPI_Scan with count=4 and MPI_SUM is executed across all processes.
 *   Each column's prefix sum across preceding rows is calculated in parallel.
 *   The transformed rows are gathered at Rank 0 using MPI_Gather and printed.
 *
 * Sample Verification:
 *   Input Matrix:
 *     1 2 3 4
 *     1 2 3 1
 *     1 1 1 1
 *     2 1 2 1
 *   Expected Output Matrix:
 *     1 2 3 4
 *     2 4 6 5
 *     3 5 7 6
 *     5 6 9 7
 *
 * Process Requirement:
 *   Requires exactly 4 processes: mpirun -np 4 ./matrix_prefix_scan
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 4

int main(int argc, char *argv[]) {
    int rank, size;
    int matrix[N][N];
    int local_row[N];
    int prefix_row[N];
    int result_matrix[N][N];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 4) {
        if (rank == 0) {
            printf("Error: This program requires exactly 4 processes (1 per row of 4x4 matrix).\n");
            printf("Usage: mpirun -np 4 %s\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        printf("============================================================\n");
        printf("   MPI Program 3: 4x4 Matrix Prefix Sum Using MPI_Scan      \n");
        printf("============================================================\n");
        printf("Enter 16 elements for the 4x4 matrix row-wise:\n");
        for (int i = 0; i < N; i++) {
            printf("  Row %d (4 integers): ", i);
            for (int j = 0; j < N; j++) {
                scanf("%d", &matrix[i][j]);
            }
        }

        printf("\nOriginal Input Matrix:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%4d ", matrix[i][j]);
            }
            printf("\n");
        }
    }

    // 1. Scatter one row (4 integers) to each of the 4 processes
    MPI_Scatter(matrix, N, MPI_INT,
                local_row, N, MPI_INT,
                0, MPI_COMM_WORLD);

    // 2. Perform element-wise prefix scan across rows using MPI_Scan
    // Each column position (0 to 3) is accumulated across preceding ranks
    MPI_Scan(local_row, prefix_row, N, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    // 3. Gather the prefix sum rows back into the result matrix at Rank 0
    MPI_Gather(prefix_row, N, MPI_INT,
               result_matrix, N, MPI_INT,
               0, MPI_COMM_WORLD);

    // 4. Display result at Rank 0
    if (rank == 0) {
        printf("\nResulting Column-Wise Prefix Sum Matrix (MPI_Scan):\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%4d ", result_matrix[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Finalize();
    return 0;
}

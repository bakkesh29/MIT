/*
 * HPCS Lab - Week 4: MPI Reduce and Scan
 * Program 2: 3x3 Matrix Element Search Using MPI_Reduce
 *
 * Description:
 *   Rank 0 reads a 3x3 matrix and a target search value.
 *   The 3 rows are distributed among 3 MPI processes (1 row of 3 elements each)
 *   using MPI_Scatter.
 *   Each process counts the occurrences of the target in its assigned row.
 *   MPI_Reduce with MPI_SUM is used to aggregate the local match counts
 *   at Rank 0 to obtain the total occurrences across the whole matrix.
 *
 * Process Requirement:
 *   Requires exactly 3 processes: mpirun -np 3 ./matrix_search_reduce
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROWS 3
#define COLS 3

int main(int argc, char *argv[]) {
    int rank, size;
    int matrix[ROWS][COLS];
    int local_row[COLS];
    int target = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 3) {
        if (rank == 0) {
            printf("Error: This program requires exactly 3 processes (1 per matrix row).\n");
            printf("Usage: mpirun -np 3 %s\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        printf("============================================================\n");
        printf("   MPI Program 2: 3x3 Matrix Search Using MPI_Reduce        \n");
        printf("============================================================\n");
        printf("Enter 9 elements for the 3x3 matrix row-wise:\n");
        for (int i = 0; i < ROWS; i++) {
            printf("  Row %d (3 elements): ", i);
            for (int j = 0; j < COLS; j++) {
                scanf("%d", &matrix[i][j]);
            }
        }

        printf("Enter target value to search: ");
        if (scanf("%d", &target) != 1) {
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    // 1. Broadcast the search target to all 3 processes
    MPI_Bcast(&target, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // 2. Scatter 1 row (3 integers) to each process
    MPI_Scatter(matrix, COLS, MPI_INT,
                local_row, COLS, MPI_INT,
                0, MPI_COMM_WORLD);

    // 3. Count occurrences in local row
    int local_count = 0;
    for (int j = 0; j < COLS; j++) {
        if (local_row[j] == target) {
            local_count++;
        }
    }

    printf("[Process %d] Row elements: [%d, %d, %d] -> Found target %d: %d time(s)\n",
           rank, local_row[0], local_row[1], local_row[2], target, local_count);

    // 4. Reduce local counts to total count at Rank 0
    int total_count = 0;
    MPI_Reduce(&local_count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // 5. Rank 0 displays final search results
    if (rank == 0) {
        printf("\n--- Search Summary ---\n");
        printf("Target Value        : %d\n", target);
        printf("Total Occurrences   : %d\n", total_count);
        if (total_count > 0) {
            printf("Status              : Target was found in the matrix!\n");
        } else {
            printf("Status              : Target was NOT found in the matrix.\n");
        }
    }

    MPI_Finalize();
    return 0;
}

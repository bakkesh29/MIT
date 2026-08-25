#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank, size;
    int matrix[3][3];
    int row[3];
    int target;
    int local_count = 0;
    int total_count = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Ensure we have exactly or at least 3 processes to handle the 3 rows
    if (size < 3) {
        if (rank == 0) {
            printf("Error: This program requires at least 3 processes to search a 3x3 matrix.\n");
        }
        MPI_Finalize();
        return 0;
    }

    // Step 1: Root process (Rank 0) reads the matrix and the target element
    if (rank == 0) {
        printf("Enter the elements of the 3x3 matrix:\n");
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                scanf("%d", &matrix[i][j]);
            }
        }
        printf("Enter the element to be searched: ");
        scanf("%d", &target);
    }

    // Step 2: Broadcast the target element from root to all processes
    MPI_Bcast(&target, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Step 3: Scatter one row of the matrix to each of the 3 processes
    // Root distributes 3 elements (1 row) to each process's local 'row' array
    MPI_Scatter(matrix, 3, MPI_INT, row, 3, MPI_INT, 0, MPI_COMM_WORLD);

    // Step 4: Each process searches its assigned row (Process 0 handles row 0, Process 1 handles row 1, etc.)
    if (rank < 3) {
        for (int j = 0; j < 3; j++) {
            if (row[j] == target) {
                local_count++;
            }
        }
    }

    // Step 5: Gather and sum up all local counts into total_count at the root process
    MPI_Reduce(&local_count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Step 6: Root process prints the final result
    if (rank == 0) {
        printf("\nResult: The element %d occurs %d time(s) in the matrix.\n", target, total_count);
    }

    MPI_Finalize();
    return 0;
}


#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank, size;
    int matrix[4][4];       // Full matrix buffer at root
    int result_matrix[4][4];// Output matrix buffer at root
    int local_row[4];       // Buffer for incoming scattered row
    int cumulative_row[4];  // Buffer for local row after prefix sum

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Enforce exactly 4 processes for a 4x4 matrix row distribution
    if (size < 4) {
        if (rank == 0) {
            printf("Error: This program requires exactly 4 processes.\n");
        }
        MPI_Finalize();
        return 0;
    }

    // Step 1: Root process reads the 4x4 input matrix
    if (rank == 0) {
        printf("Enter the elements of the 4x4 matrix:\n");
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                scanf("%d", &matrix[i][j]);
            }
        }
    }

    // Step 2: Scatter one row (4 integers) to each of the 4 processes
    MPI_Scatter(matrix, 4, MPI_INT, local_row, 4, MPI_INT, 0, MPI_COMM_WORLD);

    // Step 3: Perform an inclusive prefix sum across processes using MPI_Scan
    // Process 0 will keep its row. Process 1 adds its row to Process 0's row, etc.
    MPI_Scan(local_row, cumulative_row, 4, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    // Step 4: Gather the processed cumulative rows back into the result matrix at root
    MPI_Gather(cumulative_row, 4, MPI_INT, result_matrix, 4, MPI_INT, 0, MPI_COMM_WORLD);

    // Step 5: Root process prints the transformed matrix
    if (rank == 0) {
        printf("\nOutput Matrix:\n");
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                printf("%d ", result_matrix[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Finalize();
    return 0;
}


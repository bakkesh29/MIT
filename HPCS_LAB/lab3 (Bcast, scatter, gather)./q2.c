#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int M = 0;
    int *global_array = NULL;
    int *local_array = NULL;
    float *local_averages = NULL;
    float local_sum = 0.0, local_avg = 0.0, total_avg = 0.0, global_sum = 0.0;

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the rank of the current process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get the total number of processes (N)
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Step 1: Root process reads M and the NxM elements
    if (rank == 0) {
        printf("Enter the value of M (elements per process): ");
        fflush(stdout);
        if (scanf("%d", &M) != 1) {
            fprintf(stderr, "Error reading M\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Allocate memory for NxM elements
        global_array = (int *)malloc(size * M * sizeof(int));
        
        printf("Enter %d elements (%d processes x %d elements):\n", size * M, size, M);
        fflush(stdout);
        for (int i = 0; i < size * M; i++) {
            if (scanf("%d", &global_array[i]) != 1) {
                fprintf(stderr, "Error reading array elements\n");
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }

        // Allocate memory for gathering averages later
        local_averages = (float *)malloc(size * sizeof(float));
    }

    // Step 2: Broadcast the value of M from root to all other processes
    MPI_Bcast(&M, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Allocate space for the subset of data each process receives
    local_array = (int *)malloc(M * sizeof(int));

    // Step 3: Scatter the NxM elements so that each process receives M elements
    MPI_Scatter(global_array, M, MPI_INT, local_array, M, MPI_INT, 0, MPI_COMM_WORLD);

    // Step 4: Each process computes the average of its M elements
    for (int i = 0; i < M; i++) {
        local_sum += local_array[i];
    }
    local_avg = local_sum / M;

    printf("Process %d calculated local average: %.2f\n", rank, local_avg);
    fflush(stdout);

    // Step 5: Gather all local averages back to the root process
    MPI_Gather(&local_avg, 1, MPI_FLOAT, local_averages, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Step 6: Root process computes the final total average
    if (rank == 0) {
        for (int i = 0; i < size; i++) {
            global_sum += local_averages[i];
        }
        total_avg = global_sum / size;

        printf("\n--- Final Result ---\n");
        printf("Total average of all process averages: %.2f\n", total_avg);
        
        // Clean up root-allocated memory
        free(global_array);
        free(local_averages);
    }

    // Clean up local memory across all processes
    free(local_array);

    // Finalize the MPI environment
    MPI_Finalize();
    return 0;
}


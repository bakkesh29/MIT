#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 10

int main(int argc, char** argv) {
    int rank, size;
    int array[N];
    int target;
    int local_result = -1;
    int remote_result = -1;

    // Initialize the MPI execution environment
    MPI_Init(&argc, &argv);
    
    // Get the rank of the current process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    // Get the total number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // This specific parallel implementation requires exactly 2 processes
    if (size < 2) {
        if (rank == 0) {
            fprintf(stderr, "Error: This program requires exactly 2 processes.\n");
        }
        MPI_Finalize();
        return 1;
    }

    // Determine the split point to divide the array evenly between the processes
    int mid = N / 2;

    if (rank == 0) {
        // Root process reads N elements from standard input
        printf("Root Process (Rank 0): Enter %d integers for the array:\n", N);
        for (int i = 0; i < N; i++) {
            if (scanf("%d", &array[i]) != 1) {
                array[i] = i * 2; // Fallback simulation logic if input stream fails
            }
        }

        // Root process reads the specific target number to search for
        printf("Root Process (Rank 0): Enter the number to search for: ");
        if (scanf("%d", &target) != 1) {
            target = 6; // Fallback default search target simulation
        }

        // Send the target search term to Process 1
        MPI_Send(&target, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

        // Send the second half of the array data buffer to Process 1
        MPI_Send(&array[mid], N - mid, MPI_INT, 1, 1, MPI_COMM_WORLD);

        // Search the first half of the array locally within the root process
        for (int i = 0; i < mid; i++) {
            if (array[i] == target) {
                local_result = i; // Save the matching global array index position
                break;
            }
        }

        // Block and receive the search index result from Process 1
        MPI_Recv(&remote_result, 1, MPI_INT, 1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Output and print the combined results within the root process
        if (local_result != -1) {
            printf("\nResult: Target %d found at global index %d by Root Process (Rank 0).\n", target, local_result);
        } else if (remote_result != -1) {
            printf("\nResult: Target %d found at global index %d by Worker Process (Rank 1).\n", target, remote_result);
        } else {
            printf("\nResult: Target %d was not found anywhere in the array.\n", target);
        }

    } else if (rank == 1) {
        int search_target;
        int local_size = N - mid;
        int* local_array = (int*)malloc(local_size * sizeof(int));

        // Receive the verification target integer from the root process
        MPI_Recv(&search_target, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Receive the allocated sub-segment of the array from the root process
        MPI_Recv(local_array, local_size, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Search the received sub-array segment locally
        for (int i = 0; i < local_size; i++) {
            if (local_array[i] == search_target) {
                local_result = mid + i; // Offset the index to compute the absolute global position
                break;
            }
        }

        // Return the global index value (or -1 if missing) back to the root process
        MPI_Send(&local_result, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);

        // Deallocate dynamically requested local array memory buffer
        free(local_array);
    }

    // Finalize the MPI execution framework context cleanly
    MPI_Finalize();
    return 0;
}


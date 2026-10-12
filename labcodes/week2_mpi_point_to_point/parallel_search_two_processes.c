/*
 * HPCS Lab - Week 2: MPI Point-to-Point Communication
 * Program 4: Parallel Array Search Using Two Processes
 *
 * Description:
 *   Rank 0 reads an array of size N and a search target value.
 *   The array is partitioned between Rank 0 and Rank 1.
 *   Both processes search their respective partitions in parallel.
 *   Rank 1 transmits its search result back to Rank 0, which displays
 *   whether the target was found and at which exact index/indices.
 *
 * Execution:
 *   mpirun -np 2 ./parallel_search_two_processes
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        if (rank == 0) {
            printf("Error: This program requires exactly 2 processes.\n");
            printf("Usage: mpirun -np 2 %s\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        int n, target;

        printf("============================================================\n");
        printf("   MPI Program 4: Parallel Array Search (2 Processes)       \n");
        printf("============================================================\n");
        printf("Enter number of elements in array (N): ");
        if (scanf("%d", &n) != 1 || n <= 0) {
            printf("Invalid array size!\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        int *arr = (int *)malloc(n * sizeof(int));
        if (arr == NULL) {
            printf("Memory allocation failed!\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        printf("Enter %d integers:\n", n);
        for (int i = 0; i < n; i++) {
            scanf("%d", &arr[i]);
        }

        printf("Enter target value to search for: ");
        if (scanf("%d", &target) != 1) {
            printf("Invalid target value!\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Partition work
        int half = n / 2;
        int rank0_count = half;
        int rank1_count = n - half;
        int rank1_offset = half;

        // Send search metadata and data to Rank 1
        MPI_Send(&target, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send(&rank1_count, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
        MPI_Send(&rank1_offset, 1, MPI_INT, 1, 2, MPI_COMM_WORLD);
        MPI_Send(&arr[half], rank1_count, MPI_INT, 1, 3, MPI_COMM_WORLD);

        // Rank 0 searches its own partition [0 .. rank0_count - 1]
        int rank0_found_idx = -1;
        for (int i = 0; i < rank0_count; i++) {
            if (arr[i] == target) {
                rank0_found_idx = i;
                break; // Found first occurrence in partition
            }
        }

        // Receive result from Rank 1
        int rank1_found_idx = -1;
        MPI_Recv(&rank1_found_idx, 1, MPI_INT, 1, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Display results
        printf("\n--- Search Results ---\n");
        int found = 0;

        if (rank0_found_idx != -1) {
            printf("Found target %d in Rank 0's partition at global index %d.\n",
                   target, rank0_found_idx);
            found = 1;
        }

        if (rank1_found_idx != -1) {
            printf("Found target %d in Rank 1's partition at global index %d.\n",
                   target, rank1_found_idx);
            found = 1;
        }

        if (!found) {
            printf("Target %d was NOT found anywhere in the array.\n", target);
        }

        free(arr);

    } else if (rank == 1) {
        int target, count, offset;

        // Receive parameters from Rank 0
        MPI_Recv(&target, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&count, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int *sub_arr = (int *)malloc(count * sizeof(int));
        if (sub_arr == NULL) {
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        MPI_Recv(sub_arr, count, MPI_INT, 0, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Search in sub_arr
        int found_idx = -1;
        for (int i = 0; i < count; i++) {
            if (sub_arr[i] == target) {
                found_idx = offset + i; // Map to global index
                break;
            }
        }

        // Send found index back to Rank 0 (-1 if not found)
        MPI_Send(&found_idx, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);

        free(sub_arr);
    }

    MPI_Finalize();
    return 0;
}

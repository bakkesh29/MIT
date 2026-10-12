/*
 * HPCS Lab - Week 2: MPI Point-to-Point Communication
 * Program 3: Parallel Array Sum Using Two Processes
 *
 * Description:
 *   Rank 0 reads an array of size N, divides the array elements equally
 *   between Rank 0 and Rank 1 using point-to-point MPI_Send/MPI_Recv.
 *   Both processes compute their local sums in parallel.
 *   Rank 1 transmits its partial sum back to Rank 0, which calculates
 *   and prints the overall total sum.
 *
 * Execution:
 *   mpirun -np 2 ./array_sum_two_processes
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
        int n;

        printf("============================================================\n");
        printf("   MPI Program 3: Array Sum Divided Between 2 Processes     \n");
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

        // Divide array between Rank 0 and Rank 1
        int half = n / 2;
        int rank0_count = half;
        int rank1_count = n - half;

        // Send rank1_count and the second half of the array to Rank 1
        MPI_Send(&rank1_count, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send(&arr[half], rank1_count, MPI_INT, 1, 1, MPI_COMM_WORLD);

        // Rank 0 computes sum of its assigned half (indices 0 to half - 1)
        long long local_sum = 0;
        for (int i = 0; i < rank0_count; i++) {
            local_sum += arr[i];
        }
        printf("[Rank 0] Computed local partial sum of %d elements = %lld\n",
               rank0_count, local_sum);

        // Receive partial sum from Rank 1
        long long rank1_sum = 0;
        MPI_Recv(&rank1_sum, 1, MPI_LONG_LONG, 1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("[Rank 0] Received partial sum from Rank 1 = %lld\n", rank1_sum);

        // Final total sum
        long long total_sum = local_sum + rank1_sum;
        printf("\n--- Result ---\n");
        printf("Total Sum of Array = %lld\n", total_sum);

        free(arr);

    } else if (rank == 1) {
        int count;
        // Receive portion size
        MPI_Recv(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int *sub_arr = (int *)malloc(count * sizeof(int));
        if (sub_arr == NULL) {
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Receive second half elements
        MPI_Recv(sub_arr, count, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Compute local sum
        long long local_sum = 0;
        for (int i = 0; i < count; i++) {
            local_sum += sub_arr[i];
        }

        // Send partial sum back to Rank 0
        MPI_Send(&local_sum, 1, MPI_LONG_LONG, 0, 2, MPI_COMM_WORLD);

        free(sub_arr);
    }

    MPI_Finalize();
    return 0;
}

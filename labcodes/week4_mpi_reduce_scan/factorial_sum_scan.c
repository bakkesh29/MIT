/*
 * HPCS Lab - Week 4: MPI Reduce and Scan
 * Program 1: Factorial Sum Using MPI_Scan (1! + 2! + ... + N!)
 *
 * Description:
 *   With N processes running, each process (rank 0 to N-1) is assigned
 *   the value (rank + 1).
 *   Each process calculates the factorial of its assigned value: (rank + 1)!.
 *   MPI_Scan is used to calculate the inclusive prefix sum of factorials
 *   across all processes.
 *   Process (N - 1) obtains the grand total sum: 1! + 2! + ... + N!
 *   Execution time is measured using MPI_Wtime().
 *
 * Note:
 *   Keep N <= 12 to avoid 64-bit integer overflow.
 *
 * Execution:
 *   mpirun -np 4 ./factorial_sum_scan
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Helper function to calculate factorial
long long computeFactorial(int n) {
    long long fact = 1;
    for (int i = 2; i <= n; i++) {
        fact *= i;
    }
    return fact;
}

int main(int argc, char *argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("============================================================\n");
        printf("   MPI Program 1: Factorial Prefix Sum Using MPI_Scan       \n");
        printf("============================================================\n");
        printf("Total active processes (N) = %d\n", size);
        printf("Computing series: 1! + 2! + ... + %d!\n\n", size);
    }

    // Synchronize processes before timing
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    // 1. Each process computes factorial of its assigned number (rank + 1)
    int my_number = rank + 1;
    long long my_factorial = computeFactorial(my_number);

    // 2. Compute prefix sum of factorials using MPI_Scan
    long long prefix_sum = 0;
    MPI_Scan(&my_factorial, &prefix_sum, 1, MPI_LONG_LONG, MPI_SUM, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();
    double local_elapsed = end_time - start_time;

    // 3. Each process displays its intermediate result
    printf("[Process %d] Number: %d -> Factorial: %lld -> Cumulative Prefix Sum: %lld (Time: %.6f s)\n",
           rank, my_number, my_factorial, prefix_sum, local_elapsed);

    // 4. Maximum time across all processes using MPI_Reduce
    double max_elapsed;
    MPI_Reduce(&local_elapsed, &max_elapsed, 1, MPI_DOUBLE, MPI_MAX, size - 1, MPI_COMM_WORLD);

    // The final process (rank == size - 1) holds the complete series sum
    if (rank == size - 1) {
        printf("\n============================================================\n");
        printf("Final Series Sum (1! + 2! + ... + %d!) = %lld\n", size, prefix_sum);
        printf("Total Elapsed Execution Time              = %.6f seconds\n", max_elapsed);
        printf("============================================================\n");
    }

    MPI_Finalize();
    return 0;
}

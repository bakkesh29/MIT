#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Function to calculate factorial sequentially
unsigned long long calculate_factorial(int n) {
    unsigned long long fact = 1;
    for (int i = 2; i <= n; i++) {
        fact *= i;
    }
    return fact;
}

int main(int argc, char** argv) {
    int rank, size;
    double start_time, end_time, local_elapsed, total_program_time;

    // Initialize MPI environment
    MPI_Init(&argc, &argv);

    // Start tracking total program wall-clock execution time
    double total_start = MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Each process tracks its own execution time for the computation block
    start_time = MPI_Wtime();

    // Process 'rank' calculates term (rank + 1)!
    int current_term = rank + 1;
    unsigned long long local_factorial = calculate_factorial(current_term);
    unsigned long long partial_prefix_sum = 0;

    // Use MPI_Scan to compute the prefix sum across all processes
    MPI_Scan(&local_factorial, &partial_prefix_sum, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, MPI_COMM_WORLD);

    // Stop tracking computation time for this process
    end_time = MPI_Wtime();
    local_elapsed = end_time - start_time;

    // Synchronize all processes to cleanly capture the absolute end boundary
    MPI_Barrier(MPI_COMM_WORLD);
    total_program_time = MPI_Wtime() - total_start;

    // Print individual process computation details and its specific time
    printf("Process %d: Calculated %d! = %llu | Prefix Sum = %llu | Computation Time = %.6f seconds\n", 
           rank, current_term, local_factorial, partial_prefix_sum, local_elapsed);

    // Let the last process print the final sum and total program execution time
    if (rank == size - 1) {
        printf("\n=======================================================\n");
        printf("Final Result of Series (1! + 2! + ... + %d!): %llu\n", size, partial_prefix_sum);
        printf("Total Program Execution Time: %.6f seconds\n", total_program_time);
        printf("=======================================================\n");
    }

    // Clean up and terminate MPI environment
    MPI_Finalize();
    return 0;
}


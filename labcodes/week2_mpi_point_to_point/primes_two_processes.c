/*
 * HPCS Lab - Week 2: MPI Point-to-Point Communication
 * Program 5: Prime Numbers Between 1 and 100 Using Two Processes
 *
 * Description:
 *   Finds all prime numbers in the range 1 to 100 using two MPI processes:
 *     - Rank 0: Checks numbers from 1 to 50.
 *     - Rank 1: Checks numbers from 51 to 100.
 *   Rank 1 transmits its discovered primes to Rank 0 using MPI_Send.
 *   Rank 0 collects and displays all prime numbers in sequential order
 *   along with the total count.
 *
 * Execution:
 *   mpirun -np 2 ./primes_two_processes
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Helper function to check if a number is prime
int isPrime(int num) {
    if (num <= 1) return 0;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return 0;
    }
    return 1;
}

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
        printf("============================================================\n");
        printf("   MPI Program 5: Prime Numbers (1 to 100) with 2 Processes \n");
        printf("============================================================\n");

        int rank0_primes[50];
        int count0 = 0;

        // Rank 0 checks range 1 to 50
        for (int i = 1; i <= 50; i++) {
            if (isPrime(i)) {
                rank0_primes[count0++] = i;
            }
        }

        // Receive primes from Rank 1 (range 51 to 100)
        int count1 = 0;
        int rank1_primes[50];
        MPI_Recv(&count1, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(rank1_primes, count1, MPI_INT, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Display results
        printf("\n[Rank 0] Discovered %d primes in range [1 .. 50]:\n", count0);
        for (int i = 0; i < count0; i++) {
            printf("%d ", rank0_primes[i]);
        }
        printf("\n");

        printf("\n[Rank 1] Discovered %d primes in range [51 .. 100]:\n", count1);
        for (int i = 0; i < count1; i++) {
            printf("%d ", rank1_primes[i]);
        }
        printf("\n");

        printf("\n--- Complete List of Primes (1 to 100) ---\n");
        for (int i = 0; i < count0; i++) printf("%d ", rank0_primes[i]);
        for (int i = 0; i < count1; i++) printf("%d ", rank1_primes[i]);
        printf("\n\nTotal primes found = %d\n", count0 + count1);

    } else if (rank == 1) {
        int rank1_primes[50];
        int count1 = 0;

        // Rank 1 checks range 51 to 100
        for (int i = 51; i <= 100; i++) {
            if (isPrime(i)) {
                rank1_primes[count1++] = i;
            }
        }

        // Send results back to Rank 0
        MPI_Send(&count1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(rank1_primes, count1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}

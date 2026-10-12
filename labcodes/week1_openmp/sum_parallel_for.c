/*
 * HPCS Lab - Week 1: OpenMP
 * Program 4: Sum of Numbers from 1 to N Using Parallel For Loop
 *
 * Description:
 *   Calculates the arithmetic sum (1 + 2 + 3 + ... + N) in parallel
 *   using an OpenMP '#pragma omp parallel for' loop.
 *   Each thread accumulates a local partial sum, and the partial sums
 *   are combined into the global total using an OpenMP critical section.
 */

#include <stdio.h>
#include <omp.h>

int main(void) {
    long long n;

    printf("============================================================\n");
    printf("   OpenMP Program 4: Sum from 1 to N (Parallel For Loop)    \n");
    printf("============================================================\n");

    printf("Enter positive integer N: ");
    if (scanf("%lld", &n) != 1 || n <= 0) {
        printf("Invalid input! Must be a positive integer.\n");
        return 1;
    }

    long long total_sum = 0;

    // Parallel region
    #pragma omp parallel
    {
        long long local_sum = 0;
        int tid = omp_get_thread_num();

        // Loop iterations distributed among threads
        #pragma omp for
        for (long long i = 1; i <= n; i++) {
            local_sum += i;
        }

        // Combine local sums safely using critical section
        #pragma omp critical
        {
            printf("[Thread %d] Computed partial sum = %lld\n", tid, local_sum);
            total_sum += local_sum;
        }
    }

    // Formula verification: Sum = n * (n + 1) / 2
    long long expected_sum = n * (n + 1) / 2;

    printf("\n--- Results ---\n");
    printf("Calculated Total Sum : %lld\n", total_sum);
    printf("Formula Check N(N+1)/2: %lld\n", expected_sum);

    if (total_sum == expected_sum) {
        printf("Verification: SUCCESS (Sum is correct!)\n");
    } else {
        printf("Verification: MISMATCH!\n");
    }

    return 0;
}

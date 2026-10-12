/*
 * HPCS Lab - Week 1: OpenMP
 * Program 5: Sum of Numbers from 1 to N Using Reduction Clause
 *
 * Description:
 *   Calculates the sum (1 + 2 + ... + N) using OpenMP's built-in
 *   'reduction(+:sum)' clause.
 *
 * How Reduction Works:
 *   OpenMP automatically creates a private copy of the reduction variable
 *   for each thread, initializes it to the operator's identity (0 for +),
 *   and automatically merges each thread's local sum into the shared
 *   variable at the end of the parallel loop without critical section overhead.
 */

#include <stdio.h>
#include <omp.h>

int main(void) {
    long long n;

    printf("============================================================\n");
    printf("   OpenMP Program 5: Sum from 1 to N (Reduction Clause)     \n");
    printf("============================================================\n");

    printf("Enter positive integer N: ");
    if (scanf("%lld", &n) != 1 || n <= 0) {
        printf("Invalid input! Must be a positive integer.\n");
        return 1;
    }

    long long sum = 0;

    // Parallel for loop with reduction clause
    #pragma omp parallel for reduction(+:sum)
    for (long long i = 1; i <= n; i++) {
        sum += i;
    }

    // Formula check
    long long expected_sum = n * (n + 1) / 2;

    printf("\n--- Results ---\n");
    printf("Reduction Total Sum  : %lld\n", sum);
    printf("Formula Check N(N+1)/2: %lld\n", expected_sum);

    if (sum == expected_sum) {
        printf("Verification: SUCCESS (Matches formula!)\n");
    } else {
        printf("Verification: MISMATCH!\n");
    }

    return 0;
}

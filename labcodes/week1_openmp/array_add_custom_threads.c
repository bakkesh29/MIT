/*
 * HPCS Lab - Week 1: OpenMP
 * Program 3: Array Addition Using User-Specified Thread Count
 *
 * Description:
 *   Reads two arrays A and B of size N, prompts the user to enter the desired
 *   number of threads, and performs element-wise addition C[i] = A[i] + B[i]
 *   in parallel using OpenMP.
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(void) {
    int n, num_threads;

    printf("============================================================\n");
    printf("   OpenMP Program 3: Array Addition (Custom Thread Count)   \n");
    printf("============================================================\n");

    // 1. Read array size
    printf("Enter number of elements (N): ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid array size!\n");
        return 1;
    }

    // 2. Read user-defined thread count
    printf("Enter number of threads to use: ");
    if (scanf("%d", &num_threads) != 1 || num_threads <= 0) {
        printf("Invalid thread count! Must be a positive integer.\n");
        return 1;
    }

    // 3. Allocate memory
    int *A = (int *)malloc(n * sizeof(int));
    int *B = (int *)malloc(n * sizeof(int));
    int *C = (int *)malloc(n * sizeof(int));

    if (A == NULL || B == NULL || C == NULL) {
        printf("Memory allocation failed!\n");
        free(A); free(B); free(C);
        return 1;
    }

    // 4. Input elements
    printf("Enter %d elements for array A:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &A[i]);
    }

    printf("Enter %d elements for array B:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &B[i]);
    }

    // 5. Parallel array addition with custom thread count
    printf("\nExecuting parallel addition with %d threads...\n", num_threads);

    #pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < n; i++) {
        C[i] = A[i] + B[i];
        #pragma omp critical
        {
            printf("[Thread %d of %d] C[%d] = %d + %d = %d\n",
                   omp_get_thread_num(), omp_get_num_threads(), i, A[i], B[i], C[i]);
        }
    }

    // 6. Display final result
    printf("\n--- Resulting Array C (A + B) ---\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", C[i]);
    }
    printf("\n");

    free(A);
    free(B);
    free(C);

    return 0;
}

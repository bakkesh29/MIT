/*
 * HPCS Lab - Week 1: OpenMP
 * Program 2: Array Addition Using CPU Core Count Threads
 *
 * Description:
 *   Reads two arrays A and B of size N, adds corresponding elements
 *   in parallel using OpenMP, where the number of threads is automatically
 *   configured based on the available CPU cores (omp_get_num_procs()).
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(void) {
    int n;

    printf("============================================================\n");
    printf("   OpenMP Program 2: Array Addition (Core-Based Threads)    \n");
    printf("============================================================\n");

    // 1. Detect available processor cores
    int num_cores = omp_get_num_procs();
    printf("Detected available CPU cores: %d\n", num_cores);

    // Set OpenMP thread count to match CPU core count
    omp_set_num_threads(num_cores);

    // 2. Read array size
    printf("Enter number of elements (N): ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid array size! Must be a positive integer.\n");
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

    // 4. Input elements of arrays A and B
    printf("Enter %d elements for array A:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &A[i]);
    }

    printf("Enter %d elements for array B:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &B[i]);
    }

    // 5. Parallel Array Addition
    printf("\nComputing array addition in parallel using %d threads...\n", num_cores);

    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        C[i] = A[i] + B[i];
        #pragma omp critical
        {
            printf("[Thread %d] C[%d] = %d + %d = %d\n",
                   omp_get_thread_num(), i, A[i], B[i], C[i]);
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

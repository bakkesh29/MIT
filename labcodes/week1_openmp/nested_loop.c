/*
 * HPCS Lab - Week 1: OpenMP
 * Program 6: Parallelizing Nested Loops (2D Matrix Addition)
 *
 * Description:
 *   Demonstrates how to parallelize nested loops using OpenMP on a 2D matrix.
 *   
 * Loop Indices & Scoping:
 *   - Outer loop index 'i' iterates through matrix rows (0 to rows - 1).
 *   - Inner loop index 'j' iterates through matrix columns (0 to cols - 1).
 *   - The 'collapse(2)' clause collapses the two nested loops into a single
 *     flat iteration space of size (rows * cols), maximizing parallel work
 *     distribution among available threads.
 *   - Each thread computes independent matrix coordinates C[i][j] = A[i][j] + B[i][j],
 *     guaranteeing no data dependencies or race conditions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MAX_DIM 100

int main(void) {
    int rows, cols;
    int A[MAX_DIM][MAX_DIM];
    int B[MAX_DIM][MAX_DIM];
    int C[MAX_DIM][MAX_DIM];

    printf("============================================================\n");
    printf("   OpenMP Program 6: Nested Loop Matrix Addition            \n");
    printf("============================================================\n");

    // 1. Read matrix dimensions
    printf("Enter number of rows (<= %d): ", MAX_DIM);
    if (scanf("%d", &rows) != 1 || rows <= 0 || rows > MAX_DIM) {
        printf("Invalid row count!\n");
        return 1;
    }

    printf("Enter number of columns (<= %d): ", MAX_DIM);
    if (scanf("%d", &cols) != 1 || cols <= 0 || cols > MAX_DIM) {
        printf("Invalid column count!\n");
        return 1;
    }

    // 2. Read Matrix A
    printf("Enter %d elements of Matrix A (%d x %d):\n", rows * cols, rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            scanf("%d", &A[i][j]);
        }
    }

    // 3. Read Matrix B
    printf("Enter %d elements of Matrix B (%d x %d):\n", rows * cols, rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            scanf("%d", &B[i][j]);
        }
    }

    // 4. Parallel nested loop execution using collapse(2)
    printf("\nParallelizing nested loops with OpenMP (collapse(2))...\n");

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            // Each (i, j) element is computed independently by assigned thread
            C[i][j] = A[i][j] + B[i][j];
        }
    }

    // 5. Display result matrix
    printf("\n--- Resulting Matrix C (A + B) ---\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%5d ", C[i][j]);
        }
        printf("\n");
    }

    return 0;
}

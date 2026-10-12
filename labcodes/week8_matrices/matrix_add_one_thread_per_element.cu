/*
 * HPCS Lab - Week 8: Matrices
 * Program 11: Matrix Addition - One Thread Per Element
 *
 * Description:
 *   Adds two matrices A and B of size (Rows x Cols) in parallel using CUDA.
 *   Here, each thread computes exactly ONE individual element of the result matrix C.
 *   This provides the highest degree of fine-grained parallelism.
 *
 * Thread Indexing & Granularity:
 *   1D Grid of 1D Blocks:
 *     idx = blockIdx.x * blockDim.x + threadIdx.x;
 *   Total threads launched = Rows * Cols.
 *   Each thread computes: C[idx] = A[idx] + B[idx]
 *   (Corresponding 2D position is row = idx / Cols, col = idx % Cols).
 */

#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>

/* Macro for checking CUDA API function errors */
#define CHECK_CUDA_ERROR(call) do {                                           \
    cudaError_t err = call;                                                   \
    if (err != cudaSuccess) {                                                 \
        printf("CUDA Error at %s:%d - %s\n",                                  \
               __FILE__, __LINE__, cudaGetErrorString(err));                  \
        exit(EXIT_FAILURE);                                                   \
    }                                                                         \
} while (0)

/* CUDA Kernel: 1 Thread per Element */
__global__ void matrixAddElementKernel(const int *A, const int *B, int *C, int totalElements) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < totalElements) {
        C[idx] = A[idx] + B[idx];
    }
}

/* Helper function to display matrix */
void printMatrix(const char *name, const int *mat, int rows, int cols) {
    printf("\n%s (%d x %d):\n", name, rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%6d ", mat[i * cols + j]);
        }
        printf("\n");
    }
}

int main(void) {
    int rows, cols;

    printf("=================================================================\n");
    printf("   CUDA Program 11: Matrix Addition (One Thread Per Element)     \n");
    printf("=================================================================\n");

    // 1. Read matrix dimensions
    printf("Enter number of rows: ");
    if (scanf("%d", &rows) != 1 || rows <= 0) { printf("Invalid rows!\n"); return 1; }
    printf("Enter number of columns: ");
    if (scanf("%d", &cols) != 1 || cols <= 0) { printf("Invalid columns!\n"); return 1; }

    int totalElements = rows * cols;
    size_t size = totalElements * sizeof(int);

    // 2. Allocate host memory
    int *h_A = (int *)malloc(size);
    int *h_B = (int *)malloc(size);
    int *h_C = (int *)malloc(size);

    if (h_A == NULL || h_B == NULL || h_C == NULL) {
        printf("Host memory allocation failed!\n");
        free(h_A); free(h_B); free(h_C);
        return 1;
    }

    // 3. Read elements of Matrix A
    printf("\nEnter %d elements for Matrix A (%d x %d) row-wise:\n", totalElements, rows, cols);
    for (int i = 0; i < totalElements; i++) {
        scanf("%d", &h_A[i]);
    }

    // 4. Read elements of Matrix B
    printf("\nEnter %d elements for Matrix B (%d x %d) row-wise:\n", totalElements, rows, cols);
    for (int i = 0; i < totalElements; i++) {
        scanf("%d", &h_B[i]);
    }

    // 5. Allocate device memory
    int *d_A = NULL;
    int *d_B = NULL;
    int *d_C = NULL;

    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_A, size));
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_B, size));
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_C, size));

    // 6. Copy input data to device
    CHECK_CUDA_ERROR(cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice));
    CHECK_CUDA_ERROR(cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice));

    // 7. Configure grid: 1 thread per element
    int threadsPerBlock = 256;
    int blocksPerGrid = (totalElements + threadsPerBlock - 1) / threadsPerBlock;

    printf("\nTotal elements = %d -> Launching %d blocks of %d threads (%d threads total)...\n",
           totalElements, blocksPerGrid, threadsPerBlock, blocksPerGrid * threadsPerBlock);

    matrixAddElementKernel<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, totalElements);

    CHECK_CUDA_ERROR(cudaDeviceSynchronize());
    CHECK_CUDA_ERROR(cudaGetLastError());

    // 8. Copy result back to host
    CHECK_CUDA_ERROR(cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost));

    // 9. Display matrices
    printMatrix("Matrix A", h_A, rows, cols);
    printMatrix("Matrix B", h_B, rows, cols);
    printMatrix("Result Matrix C (A + B)", h_C, rows, cols);

    // 10. Free memory
    CHECK_CUDA_ERROR(cudaFree(d_A));
    CHECK_CUDA_ERROR(cudaFree(d_B));
    CHECK_CUDA_ERROR(cudaFree(d_C));

    free(h_A);
    free(h_B);
    free(h_C);

    printf("\nElement-wise matrix addition completed successfully!\n");
    return 0;
}

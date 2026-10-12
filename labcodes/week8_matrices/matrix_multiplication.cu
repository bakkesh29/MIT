/*
 * HPCS Lab - Week 8: Matrices
 * Program 8: Matrix Multiplication (C = A x B) in Parallel
 *
 * Description:
 *   Multiplies Matrix A of size (M x N) by Matrix B of size (N x P)
 *   to produce Matrix C of size (M x P) in parallel using CUDA.
 *
 * Thread Indexing:
 *   2D Grid of 2D Blocks:
 *     row = blockIdx.y * blockDim.y + threadIdx.y;
 *     col = blockIdx.x * blockDim.x + threadIdx.x;
 *   Each thread calculates one element C[row * P + col] by computing
 *   the dot product of row 'row' from matrix A and column 'col' from matrix B:
 *     sum = sum_{k=0}^{N-1} ( A[row * N + k] * B[k * P + col] )
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

/* CUDA Kernel: 2D Matrix Multiplication */
__global__ void matrixMulKernel(const int *A, const int *B, int *C, int M, int N, int P) {
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int row = blockIdx.y * blockDim.y + threadIdx.y;

    if (row < M && col < P) {
        int sum = 0;
        for (int k = 0; k < N; k++) {
            sum += A[row * N + k] * B[k * P + col];
        }
        C[row * P + col] = sum;
    }
}

/* Helper function to print a matrix */
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
    int M, N, P;

    printf("====================================================\n");
    printf("   CUDA Program 8: Parallel Matrix Multiplication   \n");
    printf("====================================================\n");

    // 1. Read matrix dimensions
    printf("Enter dimensions for Matrix A (M x N):\n");
    printf("  Rows (M)   : ");
    if (scanf("%d", &M) != 1 || M <= 0) { printf("Invalid M!\n"); return 1; }
    printf("  Columns (N): ");
    if (scanf("%d", &N) != 1 || N <= 0) { printf("Invalid N!\n"); return 1; }

    printf("Enter columns for Matrix B (P, since rows must be %d): ", N);
    if (scanf("%d", &P) != 1 || P <= 0) { printf("Invalid P!\n"); return 1; }

    size_t sizeA = M * N * sizeof(int);
    size_t sizeB = N * P * sizeof(int);
    size_t sizeC = M * P * sizeof(int);

    // 2. Allocate host memory
    int *h_A = (int *)malloc(sizeA);
    int *h_B = (int *)malloc(sizeB);
    int *h_C = (int *)malloc(sizeC);

    if (h_A == NULL || h_B == NULL || h_C == NULL) {
        printf("Host memory allocation failed!\n");
        free(h_A); free(h_B); free(h_C);
        return 1;
    }

    // 3. Read elements of Matrix A
    printf("\nEnter %d elements of Matrix A (%d x %d) row-wise:\n", M * N, M, N);
    for (int i = 0; i < M * N; i++) {
        scanf("%d", &h_A[i]);
    }

    // 4. Read elements of Matrix B
    printf("\nEnter %d elements of Matrix B (%d x %d) row-wise:\n", N * P, N, P);
    for (int i = 0; i < N * P; i++) {
        scanf("%d", &h_B[i]);
    }

    // 5. Allocate device memory
    int *d_A = NULL;
    int *d_B = NULL;
    int *d_C = NULL;

    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_A, sizeA));
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_B, sizeB));
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_C, sizeC));

    // 6. Copy input matrices to device
    CHECK_CUDA_ERROR(cudaMemcpy(d_A, h_A, sizeA, cudaMemcpyHostToDevice));
    CHECK_CUDA_ERROR(cudaMemcpy(d_B, h_B, sizeB, cudaMemcpyHostToDevice));

    // 7. Configure 2D grid and block dimensions
    dim3 threadsPerBlock(16, 16);
    dim3 blocksPerGrid((P + threadsPerBlock.x - 1) / threadsPerBlock.x,
                       (M + threadsPerBlock.y - 1) / threadsPerBlock.y);

    printf("\nLaunching 2D kernel grid (%d x %d blocks, %d x %d threads each)...\n",
           blocksPerGrid.x, blocksPerGrid.y, threadsPerBlock.x, threadsPerBlock.y);

    matrixMulKernel<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, M, N, P);

    CHECK_CUDA_ERROR(cudaDeviceSynchronize());
    CHECK_CUDA_ERROR(cudaGetLastError());

    // 8. Copy result matrix back to host
    CHECK_CUDA_ERROR(cudaMemcpy(h_C, d_C, sizeC, cudaMemcpyDeviceToHost));

    // 9. Display matrices
    printMatrix("Matrix A", h_A, M, N);
    printMatrix("Matrix B", h_B, N, P);
    printMatrix("Result Matrix C (A x B)", h_C, M, P);

    // 10. Free device and host memory
    CHECK_CUDA_ERROR(cudaFree(d_A));
    CHECK_CUDA_ERROR(cudaFree(d_B));
    CHECK_CUDA_ERROR(cudaFree(d_C));

    free(h_A);
    free(h_B);
    free(h_C);

    printf("\nMatrix multiplication completed successfully!\n");
    return 0;
}

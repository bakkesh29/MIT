/*
 * HPCS Lab - Week 8: Matrices
 * Program 13: Matrix Addition and Multiplication Using 2D Grid and 2D Block
 *
 * Description:
 *   Menu-driven CUDA program that lets the user select between:
 *     1. Matrix Addition (using 2D Grid and 2D Block)
 *     2. Matrix Multiplication (using 2D Grid and 2D Block)
 *     3. Exit
 *
 * Thread Indexing for 2D Grid & 2D Block:
 *   dim3 block(16, 16);
 *   row = blockIdx.y * blockDim.y + threadIdx.y;
 *   col = blockIdx.x * blockDim.x + threadIdx.x;
 *   Boundary check: if (row < rows && col < cols)
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

/* CUDA Kernel 1: 2D Matrix Addition */
__global__ void matAdd2DKernel(const int *A, const int *B, int *C, int rows, int cols) {
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int row = blockIdx.y * blockDim.y + threadIdx.y;

    if (row < rows && col < cols) {
        int idx = row * cols + col;
        C[idx] = A[idx] + B[idx];
    }
}

/* CUDA Kernel 2: 2D Matrix Multiplication */
__global__ void matMul2DKernel(const int *A, const int *B, int *C, int M, int N, int P) {
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

/* Helper function to display a matrix */
void printMatrix(const char *name, const int *mat, int rows, int cols) {
    printf("\n%s (%d x %d):\n", name, rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%6d ", mat[i * cols + j]);
        }
        printf("\n");
    }
}

/* Function to handle Matrix Addition */
void handleMatrixAddition(void) {
    int rows, cols;

    printf("\n--- Matrix Addition (2D Grid, 2D Block) ---\n");
    printf("Enter number of rows: ");
    if (scanf("%d", &rows) != 1 || rows <= 0) { printf("Invalid rows!\n"); return; }
    printf("Enter number of columns: ");
    if (scanf("%d", &cols) != 1 || cols <= 0) { printf("Invalid columns!\n"); return; }

    int totalElements = rows * cols;
    size_t size = totalElements * sizeof(int);

    int *h_A = (int *)malloc(size);
    int *h_B = (int *)malloc(size);
    int *h_C = (int *)malloc(size);

    if (h_A == NULL || h_B == NULL || h_C == NULL) {
        printf("Host memory allocation failed!\n");
        free(h_A); free(h_B); free(h_C);
        return;
    }

    printf("Enter %d elements of Matrix A row-wise:\n", totalElements);
    for (int i = 0; i < totalElements; i++) scanf("%d", &h_A[i]);

    printf("Enter %d elements of Matrix B row-wise:\n", totalElements);
    for (int i = 0; i < totalElements; i++) scanf("%d", &h_B[i]);

    int *d_A = NULL, *d_B = NULL, *d_C = NULL;
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_A, size));
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_B, size));
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_C, size));

    CHECK_CUDA_ERROR(cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice));
    CHECK_CUDA_ERROR(cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice));

    // 2D Block and 2D Grid
    dim3 threadsPerBlock(16, 16);
    dim3 blocksPerGrid((cols + threadsPerBlock.x - 1) / threadsPerBlock.x,
                       (rows + threadsPerBlock.y - 1) / threadsPerBlock.y);

    printf("Launching 2D Grid (%d x %d blocks) with 2D Block (%d x %d threads)...\n",
           blocksPerGrid.x, blocksPerGrid.y, threadsPerBlock.x, threadsPerBlock.y);

    matAdd2DKernel<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, rows, cols);

    CHECK_CUDA_ERROR(cudaDeviceSynchronize());
    CHECK_CUDA_ERROR(cudaGetLastError());

    CHECK_CUDA_ERROR(cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost));

    printMatrix("Matrix A", h_A, rows, cols);
    printMatrix("Matrix B", h_B, rows, cols);
    printMatrix("Result Matrix C (A + B)", h_C, rows, cols);

    CHECK_CUDA_ERROR(cudaFree(d_A));
    CHECK_CUDA_ERROR(cudaFree(d_B));
    CHECK_CUDA_ERROR(cudaFree(d_C));

    free(h_A); free(h_B); free(h_C);
}

/* Function to handle Matrix Multiplication */
void handleMatrixMultiplication(void) {
    int M, N, P;

    printf("\n--- Matrix Multiplication (2D Grid, 2D Block) ---\n");
    printf("Matrix A rows (M)   : ");
    if (scanf("%d", &M) != 1 || M <= 0) { printf("Invalid M!\n"); return; }
    printf("Matrix A cols (N)   : ");
    if (scanf("%d", &N) != 1 || N <= 0) { printf("Invalid N!\n"); return; }
    printf("Matrix B cols (P)   : ");
    if (scanf("%d", &P) != 1 || P <= 0) { printf("Invalid P!\n"); return; }

    size_t sizeA = M * N * sizeof(int);
    size_t sizeB = N * P * sizeof(int);
    size_t sizeC = M * P * sizeof(int);

    int *h_A = (int *)malloc(sizeA);
    int *h_B = (int *)malloc(sizeB);
    int *h_C = (int *)malloc(sizeC);

    if (h_A == NULL || h_B == NULL || h_C == NULL) {
        printf("Host memory allocation failed!\n");
        free(h_A); free(h_B); free(h_C);
        return;
    }

    printf("Enter %d elements of Matrix A (%d x %d) row-wise:\n", M * N, M, N);
    for (int i = 0; i < M * N; i++) scanf("%d", &h_A[i]);

    printf("Enter %d elements of Matrix B (%d x %d) row-wise:\n", N * P, N, P);
    for (int i = 0; i < N * P; i++) scanf("%d", &h_B[i]);

    int *d_A = NULL, *d_B = NULL, *d_C = NULL;
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_A, sizeA));
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_B, sizeB));
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_C, sizeC));

    CHECK_CUDA_ERROR(cudaMemcpy(d_A, h_A, sizeA, cudaMemcpyHostToDevice));
    CHECK_CUDA_ERROR(cudaMemcpy(d_B, h_B, sizeB, cudaMemcpyHostToDevice));

    // 2D Block and 2D Grid
    dim3 threadsPerBlock(16, 16);
    dim3 blocksPerGrid((P + threadsPerBlock.x - 1) / threadsPerBlock.x,
                       (M + threadsPerBlock.y - 1) / threadsPerBlock.y);

    printf("Launching 2D Grid (%d x %d blocks) with 2D Block (%d x %d threads)...\n",
           blocksPerGrid.x, blocksPerGrid.y, threadsPerBlock.x, threadsPerBlock.y);

    matMul2DKernel<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, M, N, P);

    CHECK_CUDA_ERROR(cudaDeviceSynchronize());
    CHECK_CUDA_ERROR(cudaGetLastError());

    CHECK_CUDA_ERROR(cudaMemcpy(h_C, d_C, sizeC, cudaMemcpyDeviceToHost));

    printMatrix("Matrix A", h_A, M, N);
    printMatrix("Matrix B", h_B, N, P);
    printMatrix("Result Matrix C (A x B)", h_C, M, P);

    CHECK_CUDA_ERROR(cudaFree(d_A));
    CHECK_CUDA_ERROR(cudaFree(d_B));
    CHECK_CUDA_ERROR(cudaFree(d_C));

    free(h_A); free(h_B); free(h_C);
}

int main(void) {
    int choice;

    while (1) {
        printf("\n=============================================================\n");
        printf("   CUDA Program 13: 2D Grid & 2D Block Matrix Operations     \n");
        printf("=============================================================\n");
        printf("1. Matrix Addition (2D Grid, 2D Block)\n");
        printf("2. Matrix Multiplication (2D Grid, 2D Block)\n");
        printf("3. Exit\n");
        printf("-------------------------------------------------------------\n");
        printf("Enter your choice (1-3): ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Exiting...\n");
            break;
        }

        switch (choice) {
            case 1:
                handleMatrixAddition();
                break;
            case 2:
                handleMatrixMultiplication();
                break;
            case 3:
                printf("Exiting program. Goodbye!\n");
                return 0;
            default:
                printf("Invalid option! Please enter 1, 2, or 3.\n");
        }
    }

    return 0;
}

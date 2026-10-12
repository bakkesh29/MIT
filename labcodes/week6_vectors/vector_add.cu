/*
 * HPCS Lab - Week 6: Vectors
 * Program 1: Vector Addition (A + B = C) in Parallel
 *
 * Description:
 *   Reads two arrays A and B of size N, computes their element-wise
 *   sum C[i] = A[i] + B[i] in parallel using CUDA, and displays the result.
 *
 * Thread Indexing:
 *   1D Grid of 1D Blocks:
 *   Global Thread ID: idx = blockIdx.x * blockDim.x + threadIdx.x
 *   Each thread computes exactly one element: C[idx] = A[idx] + B[idx]
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

/* CUDA Kernel: Parallel Vector Addition */
__global__ void vectorAddKernel(const int *A, const int *B, int *C, int n) {
    // Calculate global thread index in 1D grid
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    // Boundary check to ensure thread does not access beyond array length
    if (idx < n) {
        C[idx] = A[idx] + B[idx];
    }
}

int main(void) {
    int n;

    // 1. Read array size
    printf("=========================================\n");
    printf("   CUDA Program 1: Vector Addition       \n");
    printf("=========================================\n");
    printf("Enter number of elements (N): ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid array size! Must be a positive integer.\n");
        return 1;
    }

    size_t size = n * sizeof(int);

    // 2. Allocate host memory
    int *h_A = (int *)malloc(size);
    int *h_B = (int *)malloc(size);
    int *h_C = (int *)malloc(size);

    if (h_A == NULL || h_B == NULL || h_C == NULL) {
        printf("Host memory allocation failed!\n");
        free(h_A); free(h_B); free(h_C);
        return 1;
    }

    // 3. Read elements of Array A
    printf("Enter %d elements for vector A:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &h_A[i]);
    }

    // 4. Read elements of Array B
    printf("Enter %d elements for vector B:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &h_B[i]);
    }

    // 5. Allocate device memory
    int *d_A = NULL;
    int *d_B = NULL;
    int *d_C = NULL;

    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_A, size));
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_B, size));
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_C, size));

    // 6. Copy input data from Host to Device
    CHECK_CUDA_ERROR(cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice));
    CHECK_CUDA_ERROR(cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice));

    // 7. Configure execution grid and launch kernel
    int threadsPerBlock = 256;
    int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;

    printf("\nLaunching kernel with %d blocks of %d threads...\n", blocksPerGrid, threadsPerBlock);
    vectorAddKernel<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, n);

    // Wait for GPU to finish and check for execution errors
    CHECK_CUDA_ERROR(cudaDeviceSynchronize());
    CHECK_CUDA_ERROR(cudaGetLastError());

    // 8. Copy result back from Device to Host
    CHECK_CUDA_ERROR(cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost));

    // 9. Display the result
    printf("\n--- Resulting Vector C (A + B) ---\n");
    for (int i = 0; i < n; i++) {
        printf("C[%d] = %d + %d = %d\n", i, h_A[i], h_B[i], h_C[i]);
    }

    // 10. Free device and host memory
    CHECK_CUDA_ERROR(cudaFree(d_A));
    CHECK_CUDA_ERROR(cudaFree(d_B));
    CHECK_CUDA_ERROR(cudaFree(d_C));

    free(h_A);
    free(h_B);
    free(h_C);

    printf("\nComputation completed successfully!\n");
    return 0;
}

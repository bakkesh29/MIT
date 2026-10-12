/*
 * HPCS Lab - Week 8: Matrices
 * Program 12: Matrix Diagonal Transformation in Parallel
 *
 * Description:
 *   Reads an (N x N) square matrix and performs the following transformations:
 *     1. Principal diagonal elements (row == col) -> Set to 0.
 *     2. Elements above the diagonal (row < col)   -> Replace with their factorial.
 *     3. Elements below the diagonal (row > col)   -> Replace with their sum of digits.
 *
 * Thread Indexing:
 *   2D Grid of 2D Blocks:
 *     row = blockIdx.y * blockDim.y + threadIdx.y;
 *     col = blockIdx.x * blockDim.x + threadIdx.x;
 *   Each thread maps to matrix coordinates (row, col) and applies the
 *   appropriate transformation rule independently in parallel.
 *
 * Note on Factorial:
 *   For demonstration purposes, input numbers above the diagonal should be
 *   small integers (e.g., 1 to 5) so the factorial fits in standard 32-bit int.
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

/* Device helper: compute factorial of an integer */
__device__ int computeFactorial(int n) {
    if (n <= 1) return 1;
    int fact = 1;
    for (int i = 2; i <= n; i++) {
        fact *= i;
    }
    return fact;
}

/* Device helper: compute sum of digits of an integer */
__device__ int computeSumOfDigits(int n) {
    if (n < 0) n = -n;
    int sum = 0;
    while (n > 0) {
        sum += n % 10;
        n /= 10;
    }
    return sum;
}

/* CUDA Kernel: 2D Diagonal Transformation */
__global__ void diagonalTransformationKernel(const int *d_in, int *d_out, int n) {
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int row = blockIdx.y * blockDim.y + threadIdx.y;

    if (row < n && col < n) {
        int idx = row * n + col;
        int val = d_in[idx];

        if (row == col) {
            // Principal diagonal: set to 0
            d_out[idx] = 0;
        } else if (row < col) {
            // Above diagonal: replace with factorial
            d_out[idx] = computeFactorial(val);
        } else {
            // Below diagonal: replace with sum of digits
            d_out[idx] = computeSumOfDigits(val);
        }
    }
}

/* Helper function to display matrix */
void printMatrix(const char *title, const int *mat, int n) {
    printf("\n%s (%d x %d):\n", title, n, n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%6d ", mat[i * n + j]);
        }
        printf("\n");
    }
}

int main(void) {
    int n;

    printf("===============================================================\n");
    printf("   CUDA Program 12: Matrix Diagonal Special Transformation     \n");
    printf("===============================================================\n");

    // 1. Read matrix dimension N
    printf("Enter dimension of square matrix (N): ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid matrix dimension! Must be a positive integer.\n");
        return 1;
    }

    int totalElements = n * n;
    size_t size = totalElements * sizeof(int);

    // 2. Allocate host memory
    int *h_in = (int *)malloc(size);
    int *h_out = (int *)malloc(size);

    if (h_in == NULL || h_out == NULL) {
        printf("Host memory allocation failed!\n");
        free(h_in); free(h_out);
        return 1;
    }

    // 3. Read elements of matrix
    printf("\nEnter %d elements of %d x %d matrix row-wise:\n", totalElements, n, n);
    printf("(Tip: enter small values like 1-5 above diagonal for factorials)\n");
    for (int i = 0; i < totalElements; i++) {
        scanf("%d", &h_in[i]);
    }

    // 4. Allocate device memory
    int *d_in = NULL;
    int *d_out = NULL;

    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_in, size));
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_out, size));

    // 5. Copy host matrix to device
    CHECK_CUDA_ERROR(cudaMemcpy(d_in, h_in, size, cudaMemcpyHostToDevice));

    // 6. Configure 2D grid and block
    dim3 threadsPerBlock(16, 16);
    dim3 blocksPerGrid((n + threadsPerBlock.x - 1) / threadsPerBlock.x,
                       (n + threadsPerBlock.y - 1) / threadsPerBlock.y);

    printf("\nLaunching 2D kernel grid (%d x %d blocks, %d x %d threads each)...\n",
           blocksPerGrid.x, blocksPerGrid.y, threadsPerBlock.x, threadsPerBlock.y);

    diagonalTransformationKernel<<<blocksPerGrid, threadsPerBlock>>>(d_in, d_out, n);

    CHECK_CUDA_ERROR(cudaDeviceSynchronize());
    CHECK_CUDA_ERROR(cudaGetLastError());

    // 7. Copy transformed matrix back to host
    CHECK_CUDA_ERROR(cudaMemcpy(h_out, d_out, size, cudaMemcpyDeviceToHost));

    // 8. Display original and transformed matrices
    printMatrix("Original Matrix", h_in, n);
    printMatrix("Transformed Matrix", h_out, n);

    printf("\nTransformation Summary:\n");
    printf("- Diagonal (row == col) : Replaced with 0\n");
    printf("- Above diagonal (row < col) : Replaced with factorial\n");
    printf("- Below diagonal (row > col) : Replaced with sum of digits\n");

    // 9. Free memory
    CHECK_CUDA_ERROR(cudaFree(d_in));
    CHECK_CUDA_ERROR(cudaFree(d_out));

    free(h_in);
    free(h_out);

    printf("\nDiagonal transformation completed successfully!\n");
    return 0;
}

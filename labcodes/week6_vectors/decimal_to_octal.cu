/*
 * HPCS Lab - Week 6: Vectors
 * Program 2: Decimal to Octal Conversion in Parallel
 *
 * Description:
 *   Reads N decimal integer values, converts each value to its octal
 *   equivalent in parallel using CUDA, and displays the results.
 *
 * Thread Indexing:
 *   1D Grid of 1D Blocks:
 *   Global Thread ID: idx = blockIdx.x * blockDim.x + threadIdx.x
 *   Each thread converts one decimal integer d_dec[idx] to its octal representation
 *   stored as an integer in d_octal[idx].
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

/* CUDA Kernel: Convert each decimal integer to octal format */
__global__ void decimalToOctalKernel(const int *d_dec, int *d_octal, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < n) {
        int val = d_dec[idx];
        int sign = (val < 0) ? -1 : 1;
        if (val < 0) {
            val = -val;
        }

        if (val == 0) {
            d_octal[idx] = 0;
        } else {
            int octal = 0;
            int place = 1;
            while (val > 0) {
                int rem = val % 8;
                octal += rem * place;
                place *= 10;
                val /= 8;
            }
            d_octal[idx] = sign * octal;
        }
    }
}

int main(void) {
    int n;

    // 1. Read array size
    printf("=============================================\n");
    printf("  CUDA Program 2: Decimal to Octal Converter \n");
    printf("=============================================\n");
    printf("Enter number of integers (N): ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid input size! Must be a positive integer.\n");
        return 1;
    }

    size_t size = n * sizeof(int);

    // 2. Allocate host memory
    int *h_dec = (int *)malloc(size);
    int *h_octal = (int *)malloc(size);

    if (h_dec == NULL || h_octal == NULL) {
        printf("Host memory allocation failed!\n");
        free(h_dec); free(h_octal);
        return 1;
    }

    // 3. Read decimal integers from user
    printf("Enter %d decimal integer values:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &h_dec[i]);
    }

    // 4. Allocate device memory
    int *d_dec = NULL;
    int *d_octal = NULL;

    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_dec, size));
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_octal, size));

    // 5. Copy host decimal array to device
    CHECK_CUDA_ERROR(cudaMemcpy(d_dec, h_dec, size, cudaMemcpyHostToDevice));

    // 6. Kernel launch configuration
    int threadsPerBlock = 256;
    int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;

    printf("\nLaunching kernel with %d blocks of %d threads...\n", blocksPerGrid, threadsPerBlock);
    decimalToOctalKernel<<<blocksPerGrid, threadsPerBlock>>>(d_dec, d_octal, n);

    CHECK_CUDA_ERROR(cudaDeviceSynchronize());
    CHECK_CUDA_ERROR(cudaGetLastError());

    // 7. Copy octal results back to host
    CHECK_CUDA_ERROR(cudaMemcpy(h_octal, d_octal, size, cudaMemcpyDeviceToHost));

    // 8. Display results
    printf("\n--- Conversion Results ---\n");
    printf("%-10s %-15s %-15s\n", "Index", "Decimal", "Octal");
    printf("----------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("%-10d %-15d %-15d\n", i, h_dec[i], h_octal[i]);
    }

    // 9. Free memory
    CHECK_CUDA_ERROR(cudaFree(d_dec));
    CHECK_CUDA_ERROR(cudaFree(d_octal));

    free(h_dec);
    free(h_octal);

    printf("\nConversion completed successfully!\n");
    return 0;
}

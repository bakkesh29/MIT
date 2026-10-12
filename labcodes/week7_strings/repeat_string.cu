/*
 * HPCS Lab - Week 7: Strings
 * Program 6: Repeat String N Times in Parallel
 *
 * Description:
 *   Reads a string S and an integer count N, then produces a new string
 *   consisting of S repeated N times in parallel using CUDA.
 *
 * Thread Indexing:
 *   1D Grid of 1D Blocks:
 *   Total output characters = N * strlen(S)
 *   Global Thread ID: idx = blockIdx.x * blockDim.x + threadIdx.x
 *   Each thread calculates its assigned character using modular arithmetic:
 *     d_out[idx] = d_in[idx % in_len]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cuda_runtime.h>

#define MAX_INPUT_LEN 256

/* Macro for checking CUDA API function errors */
#define CHECK_CUDA_ERROR(call) do {                                           \
    cudaError_t err = call;                                                   \
    if (err != cudaSuccess) {                                                 \
        printf("CUDA Error at %s:%d - %s\n",                                  \
               __FILE__, __LINE__, cudaGetErrorString(err));                  \
        exit(EXIT_FAILURE);                                                   \
    }                                                                         \
} while (0)

/* CUDA Kernel: Each thread copies one character using modulo indexing */
__global__ void repeatStringKernel(const char *d_in, char *d_out, int in_len, int total_len) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < total_len) {
        d_out[idx] = d_in[idx % in_len];
    }
}

int main(void) {
    char h_in[MAX_INPUT_LEN];
    int n;

    printf("====================================================\n");
    printf("   CUDA Program 6: Repeat String N Times in Parallel\n");
    printf("====================================================\n");

    // 1. Read input string
    printf("Enter string S: ");
    if (fgets(h_in, sizeof(h_in), stdin) == NULL) {
        printf("Error reading string!\n");
        return 1;
    }

    // Strip trailing newline
    size_t in_len = strlen(h_in);
    if (in_len > 0 && h_in[in_len - 1] == '\n') {
        h_in[in_len - 1] = '\0';
        in_len--;
    }

    if (in_len == 0) {
        printf("Empty string entered!\n");
        return 0;
    }

    // 2. Read repetition count N
    printf("Enter number of repetitions (N): ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid repetition count! Must be a positive integer.\n");
        return 1;
    }

    int total_len = (int)in_len * n;
    size_t in_bytes = (in_len + 1) * sizeof(char);
    size_t out_bytes = (total_len + 1) * sizeof(char);

    // 3. Allocate host memory for output
    char *h_out = (char *)malloc(out_bytes);
    if (h_out == NULL) {
        printf("Host memory allocation failed!\n");
        return 1;
    }

    // 4. Allocate device memory
    char *d_in = NULL;
    char *d_out = NULL;

    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_in, in_bytes));
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_out, out_bytes));

    // 5. Copy input string to device
    CHECK_CUDA_ERROR(cudaMemcpy(d_in, h_in, in_bytes, cudaMemcpyHostToDevice));

    // 6. Kernel launch configuration
    int threadsPerBlock = 256;
    int blocksPerGrid = (total_len + threadsPerBlock - 1) / threadsPerBlock;

    printf("\nString length: %zu, Repetitions: %d, Total length: %d\n", in_len, n, total_len);
    printf("Launching %d blocks with %d threads each...\n", blocksPerGrid, threadsPerBlock);

    repeatStringKernel<<<blocksPerGrid, threadsPerBlock>>>(d_in, d_out, (int)in_len, total_len);

    CHECK_CUDA_ERROR(cudaDeviceSynchronize());
    CHECK_CUDA_ERROR(cudaGetLastError());

    // 7. Copy output string back to host
    CHECK_CUDA_ERROR(cudaMemcpy(h_out, d_out, total_len * sizeof(char), cudaMemcpyDeviceToHost));
    h_out[total_len] = '\0'; // Null-terminate string

    // 8. Display result
    printf("\n--- Resulting Repeated String ---\n");
    printf("%s\n", h_out);

    // 9. Free memory
    CHECK_CUDA_ERROR(cudaFree(d_in));
    CHECK_CUDA_ERROR(cudaFree(d_out));
    free(h_out);

    printf("\nExecution completed successfully!\n");
    return 0;
}

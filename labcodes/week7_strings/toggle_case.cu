/*
 * HPCS Lab - Week 7: Strings
 * Program 4: Toggle Case of String in Parallel
 *
 * Description:
 *   Reads a string and toggles the case of each English alphabet character
 *   in parallel using CUDA (uppercase -> lowercase, lowercase -> uppercase).
 *   Leaves spaces, digits, and punctuation symbols unchanged.
 *
 * Thread Indexing:
 *   1D Grid of 1D Blocks:
 *   Global Thread ID: idx = blockIdx.x * blockDim.x + threadIdx.x
 *   Each thread inspects character d_str[idx] and converts its case if alphabetic.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cuda_runtime.h>

#define MAX_STR_LEN 1024

/* Macro for checking CUDA API function errors */
#define CHECK_CUDA_ERROR(call) do {                                           \
    cudaError_t err = call;                                                   \
    if (err != cudaSuccess) {                                                 \
        printf("CUDA Error at %s:%d - %s\n",                                  \
               __FILE__, __LINE__, cudaGetErrorString(err));                  \
        exit(EXIT_FAILURE);                                                   \
    }                                                                         \
} while (0)

/* CUDA Kernel: Toggle case for each character in parallel */
__global__ void toggleCaseKernel(char *d_str, int len) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < len) {
        char ch = d_str[idx];

        // Lowercase to uppercase ('a'-'z' -> 'A'-'Z')
        if (ch >= 'a' && ch <= 'z') {
            d_str[idx] = ch - ('a' - 'A');
        }
        // Uppercase to lowercase ('A'-'Z' -> 'a'-'z')
        else if (ch >= 'A' && ch <= 'Z') {
            d_str[idx] = ch + ('a' - 'A');
        }
        // Non-alphabetic characters remain untouched
    }
}

int main(void) {
    char h_str[MAX_STR_LEN];

    printf("==================================================\n");
    printf("   CUDA Program 4: Parallel String Toggle Case    \n");
    printf("==================================================\n");
    printf("Enter a string: ");
    if (fgets(h_str, sizeof(h_str), stdin) == NULL) {
        printf("Error reading input string!\n");
        return 1;
    }

    // Strip trailing newline character
    size_t len = strlen(h_str);
    if (len > 0 && h_str[len - 1] == '\n') {
        h_str[len - 1] = '\0';
        len--;
    }

    if (len == 0) {
        printf("Empty string entered!\n");
        return 0;
    }

    printf("\nOriginal String: \"%s\" (Length: %zu)\n", h_str, len);

    // Size including the null terminator
    size_t bytes = (len + 1) * sizeof(char);

    // Allocate device memory
    char *d_str = NULL;
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_str, bytes));

    // Copy string from host to device
    CHECK_CUDA_ERROR(cudaMemcpy(d_str, h_str, bytes, cudaMemcpyHostToDevice));

    // Configure 1D grid and launch kernel
    int threadsPerBlock = 256;
    int blocksPerGrid = ((int)len + threadsPerBlock - 1) / threadsPerBlock;

    printf("Launching %d blocks with %d threads each...\n", blocksPerGrid, threadsPerBlock);
    toggleCaseKernel<<<blocksPerGrid, threadsPerBlock>>>(d_str, (int)len);

    CHECK_CUDA_ERROR(cudaDeviceSynchronize());
    CHECK_CUDA_ERROR(cudaGetLastError());

    // Copy back the modified string
    CHECK_CUDA_ERROR(cudaMemcpy(h_str, d_str, bytes, cudaMemcpyDeviceToHost));

    // Display result
    printf("\nToggled String : \"%s\"\n", h_str);

    // Free device memory
    CHECK_CUDA_ERROR(cudaFree(d_str));

    printf("\nExecution completed successfully!\n");
    return 0;
}

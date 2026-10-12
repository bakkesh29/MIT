/*
 * HPCS Lab - Week 9: CUDA Sorting
 * Program 1: Sort String Using Selection Sort on GPU
 *
 * Description:
 *   Reads an input string from the user and sorts all of its characters
 *   in ascending ASCII order using the Selection Sort algorithm executed
 *   directly on the GPU. Preserves the terminating null character ('\0')
 *   and prints the resulting sorted string.
 *
 * Example:
 *   Input : "cuda"
 *   Output: "acdu"
 *
 *   Input : "selection"
 *   Output: "ceeilnost"
 *
 * Algorithm (Selection Sort):
 *   For an unsorted string of length 'len':
 *   - Iterate index 'i' from 0 up to len - 2.
 *   - Find the index 'minIdx' of the smallest character in the range [i, len - 1].
 *   - Swap the character at position 'i' with the character at position 'minIdx'.
 *   - After len - 1 iterations, all characters in [0, len - 1] are sorted in ascending order.
 *   - The terminating null character '\0' at index 'len' remains untouched.
 *
 * Kernel & Thread Indexing:
 *   A dedicated CUDA worker thread (global thread ID idx == 0) executes the selection
 *   sort passes directly in GPU device memory (d_str).
 *   Because selection sort requires sequential progression of boundary updates and swaps,
 *   performing this loop on the GPU keeps all data movement and comparisons in high-speed
 *   device memory without incurring repeated host-device kernel launch overheads.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cuda_runtime.h>

#define MAX_STR_LEN 2048

/* Macro for checking CUDA API errors */
#define CHECK_CUDA_ERROR(call) do {                                           \
    cudaError_t err = call;                                                   \
    if (err != cudaSuccess) {                                                 \
        printf("CUDA Error at %s:%d - %s\n",                                  \
               __FILE__, __LINE__, cudaGetErrorString(err));                  \
        exit(EXIT_FAILURE);                                                   \
    }                                                                         \
} while (0)

/*
 * CUDA Kernel: Selection Sort on String Characters
 *
 * Parameters:
 *   d_str : Pointer to character array in device global memory.
 *   len   : Length of the string (excluding terminating null character).
 */
__global__ void selectionSortStringKernel(char *d_str, int len) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    // Thread 0 performs the in-place selection sort on the string in device memory
    if (idx == 0) {
        for (int i = 0; i < len - 1; i++) {
            int minIdx = i;

            // Search for the minimum character in the remaining unsorted suffix [i + 1, len - 1]
            for (int j = i + 1; j < len; j++) {
                if (d_str[j] < d_str[minIdx]) {
                    minIdx = j;
                }
            }

            // Swap characters if a smaller character was identified
            if (minIdx != i) {
                char temp = d_str[i];
                d_str[i] = d_str[minIdx];
                d_str[minIdx] = temp;
            }
        }
    }
}

int main(void) {
    char h_str[MAX_STR_LEN];

    printf("=================================================================\n");
    printf("   CUDA Program 1: Sort String Using Selection Sort on GPU       \n");
    printf("=================================================================\n");

    // 1. Read input string safely using fgets
    printf("Enter a string: ");
    if (fgets(h_str, sizeof(h_str), stdin) == NULL) {
        printf("Error: Failed to read input string.\n");
        return 1;
    }

    // 2. Strip trailing newline character if present
    size_t len = strlen(h_str);
    if (len > 0 && h_str[len - 1] == '\n') {
        h_str[len - 1] = '\0';
        len--;
    }

    // 3. Validate input
    if (len == 0) {
        printf("Error: Empty string entered.\n");
        return 0;
    }

    printf("\nOriginal String: \"%s\" (Length: %zu)\n", h_str, len);

    // Byte size including terminating null character '\0'
    size_t strBytes = (len + 1) * sizeof(char);

    // 4. Allocate device global memory
    char *d_str = NULL;
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_str, strBytes));

    // 5. Copy host string (including null character) to device
    CHECK_CUDA_ERROR(cudaMemcpy(d_str, h_str, strBytes, cudaMemcpyHostToDevice));

    // 6. Launch CUDA Kernel
    // Launching 1 block with 1 thread to perform selection sort on GPU memory
    printf("Launching Selection Sort kernel on GPU...\n");
    selectionSortStringKernel<<<1, 1>>>(d_str, (int)len);

    // Check for kernel launch errors and wait for device to finish
    CHECK_CUDA_ERROR(cudaGetLastError());
    CHECK_CUDA_ERROR(cudaDeviceSynchronize());

    // 7. Copy sorted string back from device to host
    CHECK_CUDA_ERROR(cudaMemcpy(h_str, d_str, strBytes, cudaMemcpyDeviceToHost));

    // Ensure strict null termination on host
    h_str[len] = '\0';

    // 8. Display sorted result
    printf("\n--- Result ---\n");
    printf("Sorted String  : \"%s\"\n", h_str);

    // 9. Free allocated device memory
    CHECK_CUDA_ERROR(cudaFree(d_str));

    printf("\nSorting completed successfully!\n");
    return 0;
}

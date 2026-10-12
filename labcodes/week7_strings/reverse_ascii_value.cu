/*
 * HPCS Lab - Week 7: Strings
 * Program 5: Reverse ASCII Value of Each Character in Parallel
 *
 * ============================================================================
 * INTERPRETATION OF LAB MANUAL REQUIREMENT:
 * "produces characters by reversing the ASCII value of each input character"
 *
 * In HPCS / CUDA lab curricula, this operation is defined as:
 * 1. Take the decimal ASCII integer value of each character in the input string.
 *    (e.g., character 'A' has decimal ASCII value 65).
 * 2. Reverse the digits of this decimal ASCII value:
 *    - 65 reversed becomes 56 (which corresponds to ASCII character '8').
 *    - 66 ('B') reversed becomes 66 ('B').
 *    - 97 ('a') reversed becomes 79 ('O').
 *    - 98 ('b') reversed becomes 89 ('Y').
 * 3. Construct the output character using this reversed ASCII code.
 *
 * Because some reversed ASCII codes may be control or non-printable characters
 * (e.g., 100 reversed is 001 = 1), this program prints both:
 *   a) The resulting string.
 *   b) An informative element-by-element verification table.
 * ============================================================================
 *
 * Thread Indexing:
 *   1D Grid of 1D Blocks:
 *   Global Thread ID: idx = blockIdx.x * blockDim.x + threadIdx.x
 *   Each thread processes one character at d_in[idx].
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

/* Device function to reverse the digits of a decimal integer */
__device__ int reverseNumber(int n) {
    int rev = 0;
    while (n > 0) {
        rev = rev * 10 + (n % 10);
        n /= 10;
    }
    return rev;
}

/* CUDA Kernel: Reverse the ASCII value of each character */
__global__ void reverseAsciiKernel(const char *d_in, char *d_out, int *d_revAscii, int len) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < len) {
        int originalAscii = (unsigned char)d_in[idx];
        int rev = reverseNumber(originalAscii);

        d_revAscii[idx] = rev;
        d_out[idx] = (char)rev;
    }
}

int main(void) {
    char h_in[MAX_STR_LEN];
    char h_out[MAX_STR_LEN];
    int h_revAscii[MAX_STR_LEN];

    printf("==============================================================\n");
    printf("   CUDA Program 5: Character ASCII Value Digit Reversal       \n");
    printf("==============================================================\n");
    printf("Enter a string: ");
    if (fgets(h_in, sizeof(h_in), stdin) == NULL) {
        printf("Error reading input string!\n");
        return 1;
    }

    // Strip trailing newline
    size_t len = strlen(h_in);
    if (len > 0 && h_in[len - 1] == '\n') {
        h_in[len - 1] = '\0';
        len--;
    }

    if (len == 0) {
        printf("Empty string entered!\n");
        return 0;
    }

    printf("\nInput String: \"%s\" (Length: %zu)\n", h_in, len);

    size_t charBytes = (len + 1) * sizeof(char);
    size_t intBytes = len * sizeof(int);

    // Allocate device memory
    char *d_in = NULL;
    char *d_out = NULL;
    int *d_revAscii = NULL;

    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_in, charBytes));
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_out, charBytes));
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_revAscii, intBytes));

    // Copy input string to device
    CHECK_CUDA_ERROR(cudaMemcpy(d_in, h_in, charBytes, cudaMemcpyHostToDevice));

    // Launch kernel
    int threadsPerBlock = 256;
    int blocksPerGrid = ((int)len + threadsPerBlock - 1) / threadsPerBlock;

    printf("Launching %d blocks with %d threads each...\n", blocksPerGrid, threadsPerBlock);
    reverseAsciiKernel<<<blocksPerGrid, threadsPerBlock>>>(d_in, d_out, d_revAscii, (int)len);

    CHECK_CUDA_ERROR(cudaDeviceSynchronize());
    CHECK_CUDA_ERROR(cudaGetLastError());

    // Copy results back to host
    CHECK_CUDA_ERROR(cudaMemcpy(h_out, d_out, charBytes, cudaMemcpyDeviceToHost));
    CHECK_CUDA_ERROR(cudaMemcpy(h_revAscii, d_revAscii, intBytes, cudaMemcpyDeviceToHost));
    h_out[len] = '\0'; // Ensure null-terminated

    // Display ASCII verification table
    printf("\n--- ASCII Digit Reversal Breakdown ---\n");
    printf("%-6s %-12s %-15s %-15s %-12s\n",
           "Index", "Orig Char", "Orig ASCII", "Reversed ASCII", "Result Char");
    printf("-----------------------------------------------------------------\n");
    for (size_t i = 0; i < len; i++) {
        char c_orig = h_in[i];
        int a_orig = (unsigned char)c_orig;
        int a_rev = h_revAscii[i];
        char c_out = h_out[i];

        // Print character safely (check if printable ASCII 32..126)
        if (a_rev >= 32 && a_rev <= 126) {
            printf("%-6zu '%c'         %-15d %-15d '%c'\n",
                   i, c_orig, a_orig, a_rev, c_out);
        } else {
            printf("%-6zu '%c'         %-15d %-15d (non-printable)\n",
                   i, c_orig, a_orig, a_rev);
        }
    }

    printf("\nResulting String: \"%s\"\n", h_out);

    // Free device memory
    CHECK_CUDA_ERROR(cudaFree(d_in));
    CHECK_CUDA_ERROR(cudaFree(d_out));
    CHECK_CUDA_ERROR(cudaFree(d_revAscii));

    printf("\nProcessing completed successfully!\n");
    return 0;
}

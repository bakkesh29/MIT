/*
 * HPCS Lab - Week 7: Strings
 * Program 7: Reverse Each Word in a Line in Parallel
 *
 * Description:
 *   Reads a line of text containing multiple words, reverses the characters
 *   within each individual word in parallel using CUDA, while maintaining the
 *   original order of the words and preserving all spaces.
 *
 * Example:
 *   Input : "CUDA programming is fast"
 *   Output: "ADUC gnimmargorp si tsaf"
 *
 * Thread Indexing:
 *   Total words detected = numWords.
 *   Each thread (global ID 'idx') is assigned to exactly one word:
 *     Word start index = d_starts[idx]
 *     Word end index   = d_ends[idx]
 *   The thread reverses its designated word in-place using two-pointer swapping.
 *   Because word spans do not overlap, threads execute completely independently
 *   with zero race conditions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cuda_runtime.h>

#define MAX_LINE_LEN 2048
#define MAX_WORDS 512

/* Macro for checking CUDA API function errors */
#define CHECK_CUDA_ERROR(call) do {                                           \
    cudaError_t err = call;                                                   \
    if (err != cudaSuccess) {                                                 \
        printf("CUDA Error at %s:%d - %s\n",                                  \
               __FILE__, __LINE__, cudaGetErrorString(err));                  \
        exit(EXIT_FAILURE);                                                   \
    }                                                                         \
} while (0)

/* CUDA Kernel: Reverse each word in-place */
__global__ void reverseEachWordKernel(char *d_str, const int *d_starts, const int *d_ends, int numWords) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < numWords) {
        int left = d_starts[idx];
        int right = d_ends[idx];

        while (left < right) {
            char temp = d_str[left];
            d_str[left] = d_str[right];
            d_str[right] = temp;
            left++;
            right--;
        }
    }
}

int main(void) {
    char h_str[MAX_LINE_LEN];
    int h_starts[MAX_WORDS];
    int h_ends[MAX_WORDS];

    printf("=======================================================\n");
    printf("   CUDA Program 7: Parallel Word-by-Word Reversal      \n");
    printf("=======================================================\n");
    printf("Enter a line of text: ");
    if (fgets(h_str, sizeof(h_str), stdin) == NULL) {
        printf("Error reading line!\n");
        return 1;
    }

    // Strip trailing newline
    size_t len = strlen(h_str);
    if (len > 0 && h_str[len - 1] == '\n') {
        h_str[len - 1] = '\0';
        len--;
    }

    if (len == 0) {
        printf("Empty input entered!\n");
        return 0;
    }

    // Identify word boundaries on host
    int numWords = 0;
    int inWord = 0;
    for (int i = 0; i < (int)len; i++) {
        if (h_str[i] != ' ' && h_str[i] != '\t') {
            if (!inWord) {
                h_starts[numWords] = i;
                inWord = 1;
            }
        } else {
            if (inWord) {
                h_ends[numWords] = i - 1;
                numWords++;
                inWord = 0;
                if (numWords >= MAX_WORDS) break;
            }
        }
    }
    // Close the final word if line ended inside a word
    if (inWord && numWords < MAX_WORDS) {
        h_ends[numWords] = (int)len - 1;
        numWords++;
    }

    printf("\nDetected %d word(s) in line.\n", numWords);
    printf("Original Line: \"%s\"\n", h_str);

    if (numWords == 0) {
        printf("Line contains only spaces.\n");
        return 0;
    }

    size_t strBytes = (len + 1) * sizeof(char);
    size_t wordBytes = numWords * sizeof(int);

    // Allocate device memory
    char *d_str = NULL;
    int *d_starts = NULL;
    int *d_ends = NULL;

    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_str, strBytes));
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_starts, wordBytes));
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_ends, wordBytes));

    // Copy data to device
    CHECK_CUDA_ERROR(cudaMemcpy(d_str, h_str, strBytes, cudaMemcpyHostToDevice));
    CHECK_CUDA_ERROR(cudaMemcpy(d_starts, h_starts, wordBytes, cudaMemcpyHostToDevice));
    CHECK_CUDA_ERROR(cudaMemcpy(d_ends, h_ends, wordBytes, cudaMemcpyHostToDevice));

    // Launch kernel with 1 thread per word
    int threadsPerBlock = 256;
    int blocksPerGrid = (numWords + threadsPerBlock - 1) / threadsPerBlock;

    printf("Launching %d blocks with %d threads each...\n", blocksPerGrid, threadsPerBlock);
    reverseEachWordKernel<<<blocksPerGrid, threadsPerBlock>>>(d_str, d_starts, d_ends, numWords);

    CHECK_CUDA_ERROR(cudaDeviceSynchronize());
    CHECK_CUDA_ERROR(cudaGetLastError());

    // Copy modified string back to host
    CHECK_CUDA_ERROR(cudaMemcpy(h_str, d_str, strBytes, cudaMemcpyDeviceToHost));

    // Display result
    printf("\n--- Resulting Line with Words Reversed ---\n");
    printf("\"%s\"\n", h_str);

    // Free device memory
    CHECK_CUDA_ERROR(cudaFree(d_str));
    CHECK_CUDA_ERROR(cudaFree(d_starts));
    CHECK_CUDA_ERROR(cudaFree(d_ends));

    printf("\nReversal completed successfully!\n");
    return 0;
}

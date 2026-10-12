/*
 * HPCS Lab - Week 9: CUDA Sorting
 * Program 2: Sort Each Word Using Selection Sort on GPU
 *
 * Description:
 *   Reads a line of text containing multiple words from the user.
 *   Sorts the characters within each individual word in ascending ASCII
 *   order using the Selection Sort algorithm executed in parallel on the GPU.
 *   The relative order of words and all spacing (spaces and tabs) between words
 *   are strictly preserved. The final string is null-terminated and printed.
 *
 * Example:
 *   Input : "cuda programming is fast"
 *   Output: "acdu aggimmnoprr is afst"
 *
 * Algorithm & Parallel Thread Mapping:
 *   1. Host Word Boundary Detection:
 *      The CPU scans the input line to identify start and end indices
 *      (h_starts[w] and h_ends[w]) for each distinct word.
 *   2. Thread Indexing:
 *      Total words detected = numWords.
 *      1D Grid of 1D Blocks:
 *        idx = blockIdx.x * blockDim.x + threadIdx.x
 *      Each CUDA thread is assigned to process exactly one word:
 *        Word start index = d_starts[idx]
 *        Word end index   = d_ends[idx]
 *   3. In-Kernel Selection Sort:
 *      Thread 'idx' performs selection sort exclusively over the characters
 *      in d_str[start ... end].
 *   4. Zero Race Conditions:
 *      Because words occupy non-overlapping index ranges in the string buffer,
 *      all threads execute concurrently without conflicts or synchronization
 *      hazards. Characters outside word boundaries (whitespace) are untouched.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cuda_runtime.h>

#define MAX_LINE_LEN 2048
#define MAX_WORDS 512

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
 * CUDA Kernel: Selection Sort on each individual word
 *
 * Parameters:
 *   d_str    : Pointer to line string buffer in device global memory.
 *   d_starts : Array of starting indices for each detected word.
 *   d_ends   : Array of ending indices for each detected word.
 *   numWords : Total number of words to sort in parallel.
 */
__global__ void sortEachWordSelectionKernel(char *d_str, const int *d_starts, const int *d_ends, int numWords) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < numWords) {
        int start = d_starts[idx];
        int end = d_ends[idx];

        // Selection sort on characters of the word between start and end indices (inclusive)
        for (int i = start; i < end; i++) {
            int minIdx = i;

            for (int j = i + 1; j <= end; j++) {
                if (d_str[j] < d_str[minIdx]) {
                    minIdx = j;
                }
            }

            if (minIdx != i) {
                char temp = d_str[i];
                d_str[i] = d_str[minIdx];
                d_str[minIdx] = temp;
            }
        }
    }
}

int main(void) {
    char h_str[MAX_LINE_LEN];
    int h_starts[MAX_WORDS];
    int h_ends[MAX_WORDS];

    printf("=================================================================\n");
    printf("   CUDA Program 2: Sort Each Word Using Selection Sort on GPU    \n");
    printf("=================================================================\n");

    // 1. Read input line safely using fgets
    printf("Enter a line of text: ");
    if (fgets(h_str, sizeof(h_str), stdin) == NULL) {
        printf("Error: Failed to read input line.\n");
        return 1;
    }

    // 2. Strip trailing newline character
    size_t len = strlen(h_str);
    if (len > 0 && h_str[len - 1] == '\n') {
        h_str[len - 1] = '\0';
        len--;
    }

    // 3. Validate input
    if (len == 0) {
        printf("Error: Empty line entered.\n");
        return 0;
    }

    // 4. Identify word boundaries on host
    int numWords = 0;
    int inWord = 0;

    for (int i = 0; i < (int)len; i++) {
        // Non-whitespace character marks word content
        if (h_str[i] != ' ' && h_str[i] != '\t') {
            if (!inWord) {
                h_starts[numWords] = i;
                inWord = 1;
            }
        } else {
            // Whitespace marks end of the preceding word
            if (inWord) {
                h_ends[numWords] = i - 1;
                numWords++;
                inWord = 0;
                if (numWords >= MAX_WORDS) {
                    break;
                }
            }
        }
    }

    // If the line terminates while still inside a word, close it
    if (inWord && numWords < MAX_WORDS) {
        h_ends[numWords] = (int)len - 1;
        numWords++;
    }

    printf("\nDetected %d word(s) in the line.\n", numWords);
    printf("Original Line: \"%s\"\n", h_str);

    // If line has no words (e.g., spaces only), print and exit
    if (numWords == 0) {
        printf("Notice: Line contains only whitespace characters.\n");
        return 0;
    }

    // Calculate memory buffer sizes
    size_t strBytes = (len + 1) * sizeof(char);
    size_t wordBytes = numWords * sizeof(int);

    // 5. Allocate device global memory
    char *d_str = NULL;
    int *d_starts = NULL;
    int *d_ends = NULL;

    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_str, strBytes));
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_starts, wordBytes));
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_ends, wordBytes));

    // 6. Copy string and boundary arrays to device
    CHECK_CUDA_ERROR(cudaMemcpy(d_str, h_str, strBytes, cudaMemcpyHostToDevice));
    CHECK_CUDA_ERROR(cudaMemcpy(d_starts, h_starts, wordBytes, cudaMemcpyHostToDevice));
    CHECK_CUDA_ERROR(cudaMemcpy(d_ends, h_ends, wordBytes, cudaMemcpyHostToDevice));

    // 7. Configure launch dimensions: 1 thread per word
    int threadsPerBlock = 256;
    int blocksPerGrid = (numWords + threadsPerBlock - 1) / threadsPerBlock;

    printf("Launching %d block(s) with %d threads each (total words: %d)...\n",
           blocksPerGrid, threadsPerBlock, numWords);

    sortEachWordSelectionKernel<<<blocksPerGrid, threadsPerBlock>>>(d_str, d_starts, d_ends, numWords);

    // Check for launch errors and synchronize
    CHECK_CUDA_ERROR(cudaGetLastError());
    CHECK_CUDA_ERROR(cudaDeviceSynchronize());

    // 8. Copy modified string back to host
    CHECK_CUDA_ERROR(cudaMemcpy(h_str, d_str, strBytes, cudaMemcpyDeviceToHost));
    h_str[len] = '\0';

    // 9. Display the sorted line
    printf("\n--- Resulting Line with Each Word Sorted ---\n");
    printf("\"%s\"\n", h_str);

    // 10. Free device memory
    CHECK_CUDA_ERROR(cudaFree(d_str));
    CHECK_CUDA_ERROR(cudaFree(d_starts));
    CHECK_CUDA_ERROR(cudaFree(d_ends));

    printf("\nSorting completed successfully!\n");
    return 0;
}

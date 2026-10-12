/*
 * HPCS Lab - Week 9: CUDA Sorting
 * Program 3: Odd-Even Transposition Sort on GPU
 *
 * Description:
 *   Reads an array of N integers from the user and sorts them in ascending order
 *   using the parallel Odd-Even Transposition Sort (also known as Brick Sort)
 *   algorithm on the GPU.
 *
 * Algorithm (Odd-Even Transposition Sort):
 *   Odd-Even Transposition Sort is a data-parallel sorting algorithm that alternates
 *   between two distinct phases:
 *     1. Even Phase:
 *        Compares and conditionally swaps adjacent elements at even base indices:
 *        Pairs: (0, 1), (2, 3), (4, 5), (6, 7), ...
 *     2. Odd Phase:
 *        Compares and conditionally swaps adjacent elements at odd base indices:
 *        Pairs: (1, 2), (3, 4), (5, 6), (7, 8), ...
 *
 * Convergence & Phases:
 *   A sequence of N elements is mathematically proven to be completely sorted
 *   in ascending order after at most N alternating phases (phases 0 to N - 1).
 *
 * Kernels & Thread Indexing:
 *   Two distinct CUDA kernels are defined:
 *     - evenPhaseKernel:
 *         Global thread ID idx computes base index i = 2 * idx.
 *         Compares d_arr[i] and d_arr[i + 1] if (i + 1 < n).
 *     - oddPhaseKernel:
 *         Global thread ID idx computes base index i = 2 * idx + 1.
 *         Compares d_arr[i] and d_arr[i + 1] if (i + 1 < n).
 *
 * Handling Odd and Even Lengths:
 *   The boundary check 'if (i + 1 < n)' in each kernel guarantees safe execution
 *   regardless of whether N is even or odd. If an element does not have a right
 *   neighbor within array bounds, the thread safely skips the comparison.
 *
 * Synchronization:
 *   Because the output of the even phase serves as the input to the odd phase
 *   (and vice versa), the host CPU issues cudaDeviceSynchronize() after every
 *   kernel launch to ensure complete barrier synchronization between phases.
 */

#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>

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
 * CUDA Kernel 1: Even Phase
 *
 * Each thread handles one pair starting at an even index: (2*idx, 2*idx + 1).
 */
__global__ void evenPhaseKernel(int *d_arr, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int i = 2 * idx; // Even base index: 0, 2, 4, 6, ...

    if (i + 1 < n) {
        if (d_arr[i] > d_arr[i + 1]) {
            int temp = d_arr[i];
            d_arr[i] = d_arr[i + 1];
            d_arr[i + 1] = temp;
        }
    }
}

/*
 * CUDA Kernel 2: Odd Phase
 *
 * Each thread handles one pair starting at an odd index: (2*idx + 1, 2*idx + 2).
 */
__global__ void oddPhaseKernel(int *d_arr, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int i = 2 * idx + 1; // Odd base index: 1, 3, 5, 7, ...

    if (i + 1 < n) {
        if (d_arr[i] > d_arr[i + 1]) {
            int temp = d_arr[i];
            d_arr[i] = d_arr[i + 1];
            d_arr[i + 1] = temp;
        }
    }
}

/* Helper function to display an array */
void printArray(const char *label, const int *arr, int n) {
    printf("%s: ", label);
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main(void) {
    int n;

    printf("=================================================================\n");
    printf("   CUDA Program 3: Odd-Even Transposition Sort on GPU            \n");
    printf("=================================================================\n");

    // 1. Read array size N with input validation
    printf("Enter number of elements (N): ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Error: Invalid array size. N must be a positive integer.\n");
        return 1;
    }

    // 2. Allocate host memory
    size_t bytes = n * sizeof(int);
    int *h_arr = (int *)malloc(bytes);
    if (h_arr == NULL) {
        printf("Error: Host memory allocation failed.\n");
        return 1;
    }

    // 3. Read elements of array
    printf("Enter %d integers:\n", n);
    for (int i = 0; i < n; i++) {
        if (scanf("%d", &h_arr[i]) != 1) {
            printf("Error: Failed to read element at index %d.\n", i);
            free(h_arr);
            return 1;
        }
    }

    printArray("\nOriginal Array", h_arr, n);

    // If array has only 1 element, it is already sorted
    if (n == 1) {
        printArray("Sorted Array  ", h_arr, n);
        free(h_arr);
        printf("\nSorting completed successfully!\n");
        return 0;
    }

    // 4. Allocate device global memory
    int *d_arr = NULL;
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_arr, bytes));

    // 5. Copy host array to device memory
    CHECK_CUDA_ERROR(cudaMemcpy(d_arr, h_arr, bytes, cudaMemcpyHostToDevice));

    // 6. Compute execution configuration
    // In any phase, there are at most ceil(n / 2) pairs
    int numPairs = (n + 1) / 2;
    int threadsPerBlock = 256;
    int blocksPerGrid = (numPairs + threadsPerBlock - 1) / threadsPerBlock;

    printf("\nSorting on GPU using alternating Even and Odd kernels (%d phases)...\n", n);
    printf("Grid configuration: %d block(s), %d thread(s) per block.\n",
           blocksPerGrid, threadsPerBlock);

    // 7. Execute alternating phases: N phases are sufficient to sort any array of size N
    for (int phase = 0; phase < n; phase++) {
        if (phase % 2 == 0) {
            // Even Phase: compare (0,1), (2,3), (4,5), ...
            evenPhaseKernel<<<blocksPerGrid, threadsPerBlock>>>(d_arr, n);
        } else {
            // Odd Phase: compare (1,2), (3,4), (5,6), ...
            oddPhaseKernel<<<blocksPerGrid, threadsPerBlock>>>(d_arr, n);
        }

        // Check for launch errors
        CHECK_CUDA_ERROR(cudaGetLastError());

        // Synchronize between phases to prevent race conditions
        CHECK_CUDA_ERROR(cudaDeviceSynchronize());
    }

    // 8. Copy sorted array back from device to host
    CHECK_CUDA_ERROR(cudaMemcpy(h_arr, d_arr, bytes, cudaMemcpyDeviceToHost));

    // 9. Display sorted result
    printArray("\nSorted Array  ", h_arr, n);

    // 10. Free allocated memory
    CHECK_CUDA_ERROR(cudaFree(d_arr));
    free(h_arr);

    printf("\nSorting completed successfully!\n");
    return 0;
}

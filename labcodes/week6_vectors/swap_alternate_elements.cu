/*
 * HPCS Lab - Week 6: Vectors
 * Program 3: Swap Adjacent/Alternative Elements in Parallel
 *
 * Description:
 *   Reads an integer array of size N and swaps adjacent pairs in parallel:
 *   (0, 1), (2, 3), (4, 5), and so on.
 *   If N is odd, the last unpaired element remains unchanged.
 *
 * Thread Indexing:
 *   Total pairs to swap = N / 2.
 *   Each thread (global ID 'idx') handles one unique pair:
 *     First element  = 2 * idx
 *     Second element = 2 * idx + 1
 *   Because each thread operates on non-overlapping indices,
 *   there are no race conditions or synchronization conflicts.
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

/* CUDA Kernel: Swap adjacent pairs */
__global__ void swapAlternateKernel(int *d_arr, int numPairs) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    // Check boundary within total number of pairs
    if (idx < numPairs) {
        int first = 2 * idx;
        int second = 2 * idx + 1;

        // In-place swap
        int temp = d_arr[first];
        d_arr[first] = d_arr[second];
        d_arr[second] = temp;
    }
}

int main(void) {
    int n;

    // 1. Read array size
    printf("====================================================\n");
    printf("  CUDA Program 3: Swap Alternate Adjacent Elements   \n");
    printf("====================================================\n");
    printf("Enter number of elements (N): ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid array size! Must be a positive integer.\n");
        return 1;
    }

    size_t size = n * sizeof(int);

    // 2. Allocate host memory
    int *h_arr = (int *)malloc(size);
    if (h_arr == NULL) {
        printf("Host memory allocation failed!\n");
        return 1;
    }

    // 3. Read array elements
    printf("Enter %d integers:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &h_arr[i]);
    }

    printf("\nOriginal Array:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", h_arr[i]);
    }
    printf("\n");

    // 4. Allocate device memory
    int *d_arr = NULL;
    CHECK_CUDA_ERROR(cudaMalloc((void **)&d_arr, size));

    // 5. Copy host array to device
    CHECK_CUDA_ERROR(cudaMemcpy(d_arr, h_arr, size, cudaMemcpyHostToDevice));

    // 6. Kernel launch configuration
    int numPairs = n / 2;
    if (numPairs > 0) {
        int threadsPerBlock = 256;
        int blocksPerGrid = (numPairs + threadsPerBlock - 1) / threadsPerBlock;

        printf("\nLaunching %d threads (%d blocks) for %d pairs...\n",
               numPairs, blocksPerGrid, numPairs);
        swapAlternateKernel<<<blocksPerGrid, threadsPerBlock>>>(d_arr, numPairs);

        CHECK_CUDA_ERROR(cudaDeviceSynchronize());
        CHECK_CUDA_ERROR(cudaGetLastError());
    } else {
        printf("\nArray has only 1 element, no swapping needed.\n");
    }

    // 7. Copy modified array back to host
    CHECK_CUDA_ERROR(cudaMemcpy(h_arr, d_arr, size, cudaMemcpyDeviceToHost));

    // 8. Display swapped array
    printf("\nArray After Swapping Adjacent Pairs:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", h_arr[i]);
    }
    printf("\n");

    if (n % 2 != 0) {
        printf("(Note: Last element %d at index %d was left unchanged because N is odd)\n",
               h_arr[n - 1], n - 1);
    }

    // 9. Free memory
    CHECK_CUDA_ERROR(cudaFree(d_arr));
    free(h_arr);

    printf("\nSwapping completed successfully!\n");
    return 0;
}

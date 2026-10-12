/*
 * HPCS Lab - Week 3: MPI Collectives
 * Program 3: Interleave Two Equal-Length Strings
 *
 * Description:
 *   Rank 0 reads two equal-length strings of length L.
 *   The string length L must be evenly divisible by the process count (size).
 *   Chunk size per process C = L / size.
 *   Rank 0 scatters C characters of each string to each process.
 *   Each process interleaves its matching pairs of characters:
 *     (char1[0], char2[0], char1[1], char2[1], ..., char1[C-1], char2[C-1])
 *   The interleaved chunks are gathered at Rank 0 using MPI_Gather.
 *
 * Example:
 *   String 1: "string"
 *   String 2: "length"
 *   Result  : "slternigntgh" (s-l, t-e, r-n, i-g, n-t, g-h)
 *
 * Execution:
 *   mpirun -np 2 ./interleave_strings
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define MAX_STR_LEN 256

int main(int argc, char *argv[]) {
    int rank, size;
    int chunk_size = 0;
    int str_len = 0;
    char str1[MAX_STR_LEN];
    char str2[MAX_STR_LEN];
    char *result_str = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("============================================================\n");
        printf("   MPI Program 3: String Interleaving (Scatter & Gather)     \n");
        printf("============================================================\n");
        printf("Total active processes = %d\n", size);

        printf("Enter first string (no spaces): ");
        if (scanf("%255s", str1) != 1) {
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        printf("Enter second string (equal length): ");
        if (scanf("%255s", str2) != 1) {
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        int len1 = strlen(str1);
        int len2 = strlen(str2);

        if (len1 != len2) {
            printf("Error: Strings must be of equal length! (len1 = %d, len2 = %d)\n",
                   len1, len2);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        if (len1 % size != 0) {
            printf("Error: String length (%d) must be divisible by process count (%d)!\n",
                   len1, size);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        str_len = len1;
        chunk_size = str_len / size;

        result_str = (char *)malloc((2 * str_len + 1) * sizeof(char));
    }

    // Broadcast chunk size to all processes
    MPI_Bcast(&chunk_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Allocate local buffers for each process
    char *local_str1 = (char *)malloc(chunk_size * sizeof(char));
    char *local_str2 = (char *)malloc(chunk_size * sizeof(char));
    char *local_interleaved = (char *)malloc(2 * chunk_size * sizeof(char));

    if (local_str1 == NULL || local_str2 == NULL || local_interleaved == NULL) {
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Scatter chunks of both strings
    MPI_Scatter(str1, chunk_size, MPI_CHAR,
                local_str1, chunk_size, MPI_CHAR,
                0, MPI_COMM_WORLD);

    MPI_Scatter(str2, chunk_size, MPI_CHAR,
                local_str2, chunk_size, MPI_CHAR,
                0, MPI_COMM_WORLD);

    // Each process interleaves its assigned chunk
    for (int i = 0; i < chunk_size; i++) {
        local_interleaved[2 * i]     = local_str1[i];
        local_interleaved[2 * i + 1] = local_str2[i];
    }

    // Gather interleaved chunks back at Rank 0
    MPI_Gather(local_interleaved, 2 * chunk_size, MPI_CHAR,
               result_str, 2 * chunk_size, MPI_CHAR,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        result_str[2 * str_len] = '\0'; // Null-terminate gathered string

        printf("\n--- Interleaving Result ---\n");
        printf("String 1   : %s\n", str1);
        printf("String 2   : %s\n", str2);
        printf("Interleaved: %s\n", result_str);

        free(result_str);
    }

    free(local_str1);
    free(local_str2);
    free(local_interleaved);

    MPI_Finalize();
    return 0;
}

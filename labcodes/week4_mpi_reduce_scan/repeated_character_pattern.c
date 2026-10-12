/*
 * HPCS Lab - Week 4: MPI Reduce and Scan
 * Program 4: Repeated Character Pattern Using MPI_Scatter and MPI_Gatherv
 *
 * Description:
 *   Reads a word of length N using N processes (1 process per character).
 *   MPI_Scatter distributes 1 character to each process.
 *   Each process generates a sub-string with its character repeated (rank + 1) times:
 *     - Rank 0: 1st char repeated 1 time   (e.g., 'P')
 *     - Rank 1: 2nd char repeated 2 times  (e.g., 'C' * 2 = "CC")
 *     - Rank 2: 3rd char repeated 3 times  (e.g., 'A' * 3 = "AAA")
 *     - Rank 3: 4th char repeated 4 times  (e.g., 'P' * 4 = "PPPP")
 *   Because each process produces a different number of characters (rank + 1),
 *   MPI_Gatherv (vector gather for variable counts) is used to assemble the
 *   final pattern at Rank 0.
 *
 * Example:
 *   Word    : "PCAP" (Length = 4, Requires -np 4)
 *   Pattern : "PCCAAAPPPP"
 *
 * Execution:
 *   mpirun -np 4 ./repeated_character_pattern
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define MAX_WORD_LEN 128

int main(int argc, char *argv[]) {
    int rank, size;
    char word[MAX_WORD_LEN];
    char my_char;
    int *recvcounts = NULL;
    int *displs = NULL;
    char *result_pattern = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("============================================================\n");
        printf("   MPI Program 4: Repeated Character Pattern (MPI_Gatherv)  \n");
        printf("============================================================\n");
        printf("Total active processes (N) = %d\n", size);
        printf("Enter a word of exactly %d characters: ", size);
        if (scanf("%127s", word) != 1) {
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        int len = strlen(word);
        if (len != size) {
            printf("Error: Word length (%d) must exactly match process count (%d)!\n",
                   len, size);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Set up displacement and receive count arrays for MPI_Gatherv
        recvcounts = (int *)malloc(size * sizeof(int));
        displs = (int *)malloc(size * sizeof(int));

        displs[0] = 0;
        for (int i = 0; i < size; i++) {
            recvcounts[i] = i + 1; // Rank i produces (i + 1) characters
            if (i > 0) {
                displs[i] = displs[i - 1] + recvcounts[i - 1];
            }
        }

        int total_chars = displs[size - 1] + recvcounts[size - 1];
        result_pattern = (char *)malloc((total_chars + 1) * sizeof(char));
    }

    // 1. Scatter 1 character to each of the N processes
    MPI_Scatter(word, 1, MPI_CHAR,
                &my_char, 1, MPI_CHAR,
                0, MPI_COMM_WORLD);

    // 2. Each process creates its local repeated string of length (rank + 1)
    int my_count = rank + 1;
    char *local_buf = (char *)malloc(my_count * sizeof(char));
    for (int i = 0; i < my_count; i++) {
        local_buf[i] = my_char;
    }

    // 3. Gather variable-length buffers using MPI_Gatherv at Rank 0
    MPI_Gatherv(local_buf, my_count, MPI_CHAR,
                result_pattern, recvcounts, displs, MPI_CHAR,
                0, MPI_COMM_WORLD);

    // 4. Display result at Rank 0
    if (rank == 0) {
        int total_chars = displs[size - 1] + recvcounts[size - 1];
        result_pattern[total_chars] = '\0'; // Null-terminate string

        printf("\n--- Generated Pattern Result ---\n");
        printf("Original Word : %s (Length %d)\n", word, size);
        printf("Result Pattern: %s (Length %d)\n", result_pattern, total_chars);

        free(recvcounts);
        free(displs);
        free(result_pattern);
    }

    free(local_buf);
    MPI_Finalize();
    return 0;
}

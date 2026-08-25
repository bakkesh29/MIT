#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define MAX_LEN 100

int main(int argc, char** argv) {
    int rank, size;
    char S1[MAX_LEN], S2[MAX_LEN], result[MAX_LEN * 2];
    int chunk_size = 0;
    int total_len = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        strcpy(S1, "string");
        strcpy(S2, "length");

        total_len = strlen(S1);
        chunk_size = total_len / size;
    }

    // Broadcast the chunk size to all processes
    MPI_Bcast(&chunk_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Buffers to hold the pieces received by each process
    char local_S1[MAX_LEN], local_S2[MAX_LEN], local_res[MAX_LEN * 2];

    // Distribute equal parts of S1 and S2 to all processes
    MPI_Scatter(S1, chunk_size, MPI_CHAR, local_S1, chunk_size, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Scatter(S2, chunk_size, MPI_CHAR, local_S2, chunk_size, MPI_CHAR, 0, MPI_COMM_WORLD);

    // Interleave character by character locally from the received chunks
    int idx = 0;
    for (int i = 0; i < chunk_size; i++) {
        local_res[idx++] = local_S1[i]; // Fixed: Read from local chunk
        local_res[idx++] = local_S2[i]; // Fixed: Read from local chunk
    }

    // Collect all local results back into the root's result array
    MPI_Gather(local_res, 2 * chunk_size, MPI_CHAR, result, 2 * chunk_size, MPI_CHAR, 0, MPI_COMM_WORLD);

    // Root process displays the final output
    if (rank == 0) {
        result[total_len * 2] = '\0'; // Explicitly null-terminate the final string

        printf("S1: %s\n", S1);
        printf("S2: %s\n", S2);
        printf("Resultant String: %s\n", result);
    }

    MPI_Finalize();
    return 0;
}


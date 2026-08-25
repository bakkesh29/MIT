#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int rank, size;
    int N = 0;
    char *word = NULL;
    char character;
    char *local_output;
    char *result = NULL;
    int *counts = NULL;
    int *displacements = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Enter a word: ");
        fflush(stdout);

        word = malloc((size + 1) * sizeof(char));

        if (scanf("%s", word) != 1) {
            fprintf(stderr, "Error reading word.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        N = strlen(word);

        if (N != size) {
            fprintf(stderr,
                    "Error: The word length (%d) must equal "
                    "the number of processes (%d).\n",
                    N, size);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    /* Send the word length to all processes */
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    /* Distribute one character to each process */
    MPI_Scatter(word, 1, MPI_CHAR,
                &character, 1, MPI_CHAR,
                0, MPI_COMM_WORLD);

    
    local_output = malloc((rank + 1) * sizeof(char));

    MPI_Barrier(MPI_COMM_WORLD);
    double program_start = MPI_Wtime();

    double process_start = MPI_Wtime();

    for (int i = 0; i <= rank; i++) {
        local_output[i] = character;
    }

    double process_end = MPI_Wtime();
    double process_time = process_end - process_start;

    /* Prepare receive counts and displacements at the root */
    if (rank == 0) {
        counts = malloc(N * sizeof(int));
        displacements = malloc(N * sizeof(int));

        int position = 0;

        for (int i = 0; i < N; i++) {
            counts[i] = i + 1;
            displacements[i] = position;
            position += counts[i];
        }

        result = malloc((position + 1) * sizeof(char));
    }

    /* Gather the repeated characters at the root */
    MPI_Gatherv(local_output, rank + 1, MPI_CHAR,
                result, counts, displacements, MPI_CHAR,
                0, MPI_COMM_WORLD);

    double program_end = MPI_Wtime();
    double program_time = program_end - program_start;

    /* Display results at the root */
    if (rank == 0) {
        int result_length = N * (N + 1) / 2;
        result[result_length] = '\0';

        printf("\nResultant word: %s\n", result);
        printf("Whole program time: %.9f seconds\n\n", program_time);
    }

    /* Display the time taken by each process in rank order */
    for (int i = 0; i < size; i++) {
        MPI_Barrier(MPI_COMM_WORLD);

        if (rank == i) {
            printf("Process %d generated '%c' repeated %d time(s) "
                   "in %.9f seconds\n",
                   rank, character, rank + 1, process_time);
            fflush(stdout);
        }
    }

    free(local_output);

    if (rank == 0) {
        free(word);
        free(result);
        free(counts);
        free(displacements);
    }

    MPI_Finalize();
    return 0;
}


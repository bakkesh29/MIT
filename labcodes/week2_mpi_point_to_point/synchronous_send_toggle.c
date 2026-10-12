/*
 * HPCS Lab - Week 2: MPI Point-to-Point Communication
 * Program 2: Toggle String Case Using Synchronous Send (MPI_Ssend)
 *
 * Description:
 *   Demonstrates synchronous communication using MPI_Ssend between two processes.
 *     1. Rank 0 reads an input word and transmits it to Rank 1 via MPI_Ssend.
 *     2. Rank 1 receives the word, toggles the case of every letter, and
 *        sends the transformed word back to Rank 0 using MPI_Ssend.
 *     3. Rank 0 receives and displays the toggled string.
 *
 * Deadlock Avoidance:
 *   MPI_Ssend is a synchronous blocking send that does not complete until
 *   the receiving process has actively started receiving the message.
 *   If both Rank 0 and Rank 1 attempted MPI_Ssend simultaneously, both would
 *   block indefinitely, resulting in a DEADLOCK.
 *   To guarantee safe execution:
 *     - Rank 0: Ssend -> Recv
 *     - Rank 1: Recv  -> Ssend
 *
 * Execution:
 *   mpirun -np 2 ./synchronous_send_toggle
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>

#define BUFFER_SIZE 256

int main(int argc, char *argv[]) {
    int rank, size;
    char message[BUFFER_SIZE];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Enforce exactly 2 processes
    if (size != 2) {
        if (rank == 0) {
            printf("Error: This program requires exactly 2 processes.\n");
            printf("Usage: mpirun -np 2 %s\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        printf("============================================================\n");
        printf("   MPI Program 2: Case Toggle with MPI_Ssend (No Deadlock)  \n");
        printf("============================================================\n");
        printf("Enter a word/string: ");
        if (scanf("%255s", message) != 1) {
            printf("Invalid string input!\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        int len = strlen(message) + 1; // Include null terminator

        printf("\n[Rank 0] Sending original word: \"%s\" to Rank 1 via MPI_Ssend...\n", message);
        // Step 1: Rank 0 sends to Rank 1 synchronously
        MPI_Ssend(message, len, MPI_CHAR, 1, 0, MPI_COMM_WORLD);

        // Step 2: Rank 0 waits to receive the modified word from Rank 1
        MPI_Recv(message, BUFFER_SIZE, MPI_CHAR, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("[Rank 0] Received toggled word from Rank 1: \"%s\"\n", message);

    } else if (rank == 1) {
        // Step 1: Rank 1 first receives from Rank 0 (avoids deadlock)
        MPI_Recv(message, BUFFER_SIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("[Rank 1] Received: \"%s\". Toggling letter cases...\n", message);

        // Toggle letter case
        for (int i = 0; message[i] != '\0'; i++) {
            if (islower(message[i])) {
                message[i] = toupper(message[i]);
            } else if (isupper(message[i])) {
                message[i] = tolower(message[i]);
            }
        }

        int len = strlen(message) + 1;
        printf("[Rank 1] Sending toggled word back to Rank 0 via MPI_Ssend...\n");
        // Step 2: Rank 1 sends back to Rank 0
        MPI_Ssend(message, len, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}

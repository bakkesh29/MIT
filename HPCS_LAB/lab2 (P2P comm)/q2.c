#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char* argv[]) {
    int rank;
    char word[100]; // CRITICAL FIX: Restored array brackets so it holds the full string
    int size;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    

    if (rank == 0) {
        strcpy(word, "Hello World");
        printf("Initial word is: %s\n", word);
        
        // Send string to Rank 1
        MPI_Ssend(word, strlen(word) + 1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        
        // Receive the modified string back
        MPI_Recv(word, 100, MPI_CHAR, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        printf("Toggled word: %s\n", word);
    } 
    else if (rank == 1) {
        // Receive string from Rank 0 safely
        MPI_Recv(word, 100, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Loop through the character array to invert the capitalization
        for (int i = 0; word[i] != '\0'; i++) {
            if (islower((unsigned char)word[i])) {
                word[i] = toupper((unsigned char)word[i]);
            } 
            else if (isupper((unsigned char)word[i])) {
                word[i] = tolower((unsigned char)word[i]);
            }
        }
        
        // Send the flipped "hELLO wORLD" string back to Rank 0
        MPI_Ssend(word, strlen(word) + 1, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}


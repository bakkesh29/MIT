#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool is_prime(int num) {
    if (num < 2) return false;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return false;
    }
    return true;
}

int main(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        if (rank == 0) {
            fprintf(stderr, "Error: This program must be run with exactly 2 processes.\n");
        }
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        int primes_p0[100];
        int count_p0 = 0;

        for (int i = 1; i <= 50; i++) {
            if (is_prime(i)) {
                primes_p0[count_p0++] = i;
            }
        }

        int count_p1;
        MPI_Recv(&count_p1, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int* primes_p1 = (int*)malloc(count_p1 * sizeof(int));
        MPI_Recv(primes_p1, count_p1, MPI_INT, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Process 0 primes (1 to 50):\n");
        for (int i = 0; i < count_p0; i++) {
            printf("%d ", primes_p0[i]);
        }
        printf("\n\n");

        printf("Process 1 primes (51 to 100):\n");
        for (int i = 0; i < count_p1; i++) {
            printf("%d ", primes_p1[i]);
        }
        printf("\n");

        free(primes_p1);

    } else if (rank == 1) {
        int primes_p1[100];
        int count_p1 = 0;

        for (int i = 51; i <= 100; i++) {
            if (is_prime(i)) {
                primes_p1[count_p1++] = i;
            }
        }

        int data_size = (count_p1 * sizeof(int)) + sizeof(int);
        int total_buffer_size = data_size + (2 * MPI_BSEND_OVERHEAD);
        void* bsend_buffer = malloc(total_buffer_size);

        MPI_Buffer_attach(bsend_buffer, total_buffer_size);

        MPI_Bsend(&count_p1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Bsend(primes_p1, count_p1, MPI_INT, 0, 1, MPI_COMM_WORLD);

        int detached_size;
        void* detached_buffer;
        MPI_Buffer_detach(&detached_buffer, &detached_size);

        free(detached_buffer);
    }

    MPI_Finalize();
    return 0;
}


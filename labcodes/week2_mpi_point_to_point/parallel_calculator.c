/*
 * HPCS Lab - Week 2: MPI Point-to-Point Communication
 * Program 1: Parallel Calculator Using 5 Processes
 *
 * Description:
 *   Coordinates arithmetic operations across 5 MPI processes:
 *     - Rank 0: Master/Coordinator process (reads input, distributes data, collects results)
 *     - Rank 1: Worker - Addition (a + b)
 *     - Rank 2: Worker - Subtraction (a - b)
 *     - Rank 3: Worker - Multiplication (a * b)
 *     - Rank 4: Worker - Division (a / b), with division-by-zero protection.
 *
 * Communication:
 *   Standard blocking point-to-point communication (MPI_Send and MPI_Recv).
 *   Requires exactly 5 processes: mpirun -np 5 ./parallel_calculator
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Enforce exactly 5 processes
    if (size != 5) {
        if (rank == 0) {
            printf("Error: This program requires exactly 5 processes.\n");
            printf("Usage: mpirun -np 5 %s\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        double a, b;

        printf("============================================================\n");
        printf("   MPI Program 1: 5-Process Parallel Calculator             \n");
        printf("============================================================\n");
        printf("Enter operand 1 (a): ");
        if (scanf("%lf", &a) != 1) {
            printf("Invalid input!\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        printf("Enter operand 2 (b): ");
        if (scanf("%lf", &b) != 1) {
            printf("Invalid input!\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        double operands[2] = {a, b};

        // Send operands to all 4 worker ranks
        for (int worker = 1; worker <= 4; worker++) {
            MPI_Send(operands, 2, MPI_DOUBLE, worker, 0, MPI_COMM_WORLD);
        }

        // Receive results from workers
        double add_res, sub_res, mul_res, div_res;
        int div_status; // 0 = success, 1 = division by zero

        MPI_Recv(&add_res, 1, MPI_DOUBLE, 1, 10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&sub_res, 1, MPI_DOUBLE, 2, 20, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&mul_res, 1, MPI_DOUBLE, 3, 30, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&div_status, 1, MPI_INT, 4, 40, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (div_status == 0) {
            MPI_Recv(&div_res, 1, MPI_DOUBLE, 4, 41, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Display results
        printf("\n--- Results Received by Coordinator (Rank 0) ---\n");
        printf("Rank 1 (Addition)       : %.2f + %.2f = %.2f\n", a, b, add_res);
        printf("Rank 2 (Subtraction)    : %.2f - %.2f = %.2f\n", a, b, sub_res);
        printf("Rank 3 (Multiplication) : %.2f * %.2f = %.2f\n", a, b, mul_res);
        if (div_status == 0) {
            printf("Rank 4 (Division)       : %.2f / %.2f = %.2f\n", a, b, div_res);
        } else {
            printf("Rank 4 (Division)       : Error! Division by zero is undefined.\n");
        }

    } else if (rank == 1) {
        // Worker 1: Addition
        double operands[2];
        MPI_Recv(operands, 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        double result = operands[0] + operands[1];
        MPI_Send(&result, 1, MPI_DOUBLE, 0, 10, MPI_COMM_WORLD);

    } else if (rank == 2) {
        // Worker 2: Subtraction
        double operands[2];
        MPI_Recv(operands, 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        double result = operands[0] - operands[1];
        MPI_Send(&result, 1, MPI_DOUBLE, 0, 20, MPI_COMM_WORLD);

    } else if (rank == 3) {
        // Worker 3: Multiplication
        double operands[2];
        MPI_Recv(operands, 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        double result = operands[0] * operands[1];
        MPI_Send(&result, 1, MPI_DOUBLE, 0, 30, MPI_COMM_WORLD);

    } else if (rank == 4) {
        // Worker 4: Division
        double operands[2];
        MPI_Recv(operands, 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (operands[1] == 0.0) {
            int div_status = 1; // Error
            MPI_Send(&div_status, 1, MPI_INT, 0, 40, MPI_COMM_WORLD);
        } else {
            int div_status = 0; // Success
            double result = operands[0] / operands[1];
            MPI_Send(&div_status, 1, MPI_INT, 0, 40, MPI_COMM_WORLD);
            MPI_Send(&result, 1, MPI_DOUBLE, 0, 41, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}

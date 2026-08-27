#include <stdio.h>

#define BITS 6 // 6-bit counter (0 to 63)

int A[BITS] = {0}; // Counter initialized to all 0s

void printBinary() {
    for (int i = BITS - 1; i >= 0; i--) {
        printf("%d", A[i]);
    }
}

void increment(int step) {
    int i = 0;
    int actual_cost = 0;

    // Flip trailing 1s to 0s (paid by stored credit)
    while (i < BITS && A[i] == 1) {
        A[i] = 0;
        actual_cost++; // Each 1 -> 0 flip costs 1 actual work
        i++;
    }

    // Flip the lowest 0 to 1
    if (i < BITS) {
        A[i] = 1;
        actual_cost++; // The 0 -> 1 flip costs 1 actual work
    }

    // Accounting method values:
    // Amortized cost charged per increment is always 2
    int amortized_cost = 2;
    int credit_change = amortized_cost - actual_cost;

    printf("Inc #%2d | Value: ", step);
    printBinary();
    printf(" | Actual Cost: %d | Amortized Cost: %d | Net Credit: %+d\n",
           actual_cost, amortized_cost, credit_change);
}

int main() {
    int total_increments = 16;
    int total_actual_cost = 0;
    int total_amortized_cost = 0;

    printf("========================================================================\n");
    printf("      EXPERIMENT 2: ACCOUNTING METHOD ON BINARY COUNTER (6-BIT)         \n");
    printf("========================================================================\n\n");

    printf("Initial Counter: ");
    printBinary();
    printf("\n\n");

    for (int i = 1; i <= total_increments; i++) {
        // Track theoretical cost for summary
        int temp_i = 0;
        int count_flips = 0;
        while (temp_i < BITS && A[temp_i] == 1) {
            count_flips++;
            temp_i++;
        }
        if (temp_i < BITS) count_flips++;
        
        total_actual_cost += count_flips;
        total_amortized_cost += 2;

        increment(i);
    }

    printf("\n------------------------------------------------------------------------\n");
    printf("Summary of Accounting Analysis:\n");
    printf("Total Increments (n)       : %d\n", total_increments);
    printf("Total Actual Cost          : %d\n", total_actual_cost);
    printf("Total Amortized Cost (2*n) : %d\n", total_amortized_cost);
    printf("Unused Stored Credit       : %d (Total Amortized - Total Actual >= 0)\n", 
           total_amortized_cost - total_actual_cost);
    printf("Conclusion                 : Amortized cost per increment is O(1)\n");
    printf("------------------------------------------------------------------------\n");

    return 0;
}
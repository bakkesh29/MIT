/*
 * HPCS Lab - Week 1: OpenMP
 * Program 1: Demonstration of Shared and Private Variables
 *
 * Description:
 *   Demonstrates how OpenMP handles shared and private variables inside
 *   a parallel region.
 *   - 'shared' variables are stored in common memory and accessible by all threads.
 *   - 'private' variables create a thread-local instance for each thread,
 *     independent of other threads.
 */

#include <stdio.h>
#include <omp.h>

int main(void) {
    int shared_var = 100; // Accessible and shared by all threads
    int private_var = 0;  // Will be made private to each thread

    printf("============================================================\n");
    printf("   OpenMP Program 1: Shared and Private Variables Demo      \n");
    printf("============================================================\n");

    printf("Before Parallel Region:\n");
    printf("  shared_var  = %d\n", shared_var);
    printf("  private_var = %d\n\n", private_var);

    // Fork threads with explicitly specified variable scoping
    #pragma omp parallel private(private_var) shared(shared_var)
    {
        int tid = omp_get_thread_num();
        int num_threads = omp_get_num_threads();

        // Each thread assigns a unique value to its private copy
        private_var = (tid + 1) * 10;

        #pragma omp critical
        {
            printf("[Thread %d of %d] private_var = %d, shared_var = %d\n",
                   tid, num_threads, private_var, shared_var);
            // Increment shared variable safely inside critical region
            shared_var += 5;
        }
    }

    printf("\nAfter Parallel Region:\n");
    printf("  shared_var  = %d (modified by all threads)\n", shared_var);
    printf("  private_var = %d (original master copy unchanged)\n", private_var);

    return 0;
}

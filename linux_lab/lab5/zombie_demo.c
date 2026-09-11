#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid;

    // Create a new child process
    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } 
    else if (pid == 0) {
        // Child process finishes immediately
        printf("Child Process (PID: %d) is exiting immediately to become a zombie.\n", getpid());
        exit(0);
    } 
    else {
        // Parent process sleeps, leaving the child as a zombie
        printf("Parent Process (PID: %d) is sleeping for 15 seconds.\n", getpid());
        printf("--> QUICK: Run the 'ps' command in your terminal NOW to see the <defunct> process!\n");
        
        sleep(15); 
        
        printf("\nParent Process waking up and exiting. The init process will now clean up the zombie.\n");
    }

    return 0;
}


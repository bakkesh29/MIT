#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    printf("Starting exec_demo program...\n");
    
    // Create a new child process
    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } 
    else if (pid == 0) {
        // This is the child process. 
        // We will replace it with the 'wait_demo' executable.
        printf("Child Process: Loading the 'wait_demo' executable using execl()...\n\n");
        
        // execl takes the path to the executable, the command name, and a NULL terminator
        execl("./wait_demo", "wait_demo", NULL);
        
        // If execl is successful, the code below will NEVER execute.
        // If we reach here, it means execl failed (e.g., file not found).
        perror("exec failed - Make sure './wait_demo' exists in this directory!");
        exit(1);
    } 
    else {
        // This is the parent process.
        printf("Parent Process: Waiting for the exec'd child to finish...\n");
        
        // Wait for the child to finish executing the new binary
        wait(NULL);
        
        printf("\nParent Process: The exec'd child has finished. Exiting.\n");
    }

    return 0;
}

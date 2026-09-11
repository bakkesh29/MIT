#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    // Create a new child process
    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } 
    else if (pid == 0) {
        // This block executes only in the child process
        printf("--- Inside Child Process ---\n");
        printf("My Process ID (PID)      : %d\n", getpid());
        printf("My Parent's ID (PPID)    : %d\n", getppid());
        printf("My Child's ID            : None (fork() returned %d)\n\n", pid);
        exit(0);
    } 
    else {
        // This block executes only in the parent process
        // We use wait(NULL) just to let the child print first for cleaner output
        wait(NULL); 
        
        printf("--- Inside Parent Process ---\n");
        printf("My Process ID (PID)      : %d\n", getpid());
        printf("My Parent's ID (PPID)    : %d (This is usually your terminal shell)\n", getppid());
        printf("My Child's ID            : %d\n\n", pid);
    }

    return 0;
}

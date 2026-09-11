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
        printf("Child Process: My PID is %d\n", getpid());
        printf("Child Process: Simulating some work for 2 seconds...\n");
        sleep(2); 
        printf("Child Process: Work completed. Exiting.\n");
        exit(0);
    } 
    else {
        printf("Parent Process: My PID is %d\n", getpid());
        printf("Parent Process: Waiting for child (PID: %d) to finish...\n", pid);
        
        // Block the parent process until the child completes
        wait(NULL);
        
        printf("Parent Process: Child has completed. Parent is now exiting.\n");
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
        printf("Child Process: My PID is %d\n", getpid());
        printf("Child Process: Simulating some work for 2 seconds...\n");
        sleep(2); 
        printf("Child Process: Work completed. Exiting.\n");
        exit(0);
    } 
    else {
        printf("Parent Process: My PID is %d\n", getpid());
        printf("Parent Process: Waiting for child (PID: %d) to finish...\n", pid);
        
        // Block the parent process until the child completes
        wait(NULL);
        
        printf("Parent Process: Child has completed. Parent is now exiting.\n");
    }

    return 0;
}   }

    return 0;
}

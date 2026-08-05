#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t pid1, pid2;

    // Create first child
    pid1 = fork();

    if (pid1 == 0)
    {
        printf("First Child Process\n");
        printf("PID  : %d\n", getpid());
        printf("PPID : %d\n", getppid());

        sleep(2);

        printf("First Child Completed.\n");
        exit(0);
    }

    // Create second child
    pid2 = fork();

    if (pid2 == 0)
    {
        printf("\nSecond Child Process\n");
        printf("PID  : %d\n", getpid());
        printf("PPID : %d\n", getppid());

        sleep(4);

        printf("Second Child Completed.\n");
        exit(0);
    }

    // Parent process
    printf("\nParent Process\n");
    printf("Parent PID : %d\n", getpid());

    // Wait for first child
    waitpid(pid1, NULL, 0);
    printf("First Child Collected using waitpid().\n");

    // Wait for remaining child
    wait(NULL);
    printf("Second Child Collected using wait().\n");

    printf("All Child Processes Finished.\n");

    return 0;
}

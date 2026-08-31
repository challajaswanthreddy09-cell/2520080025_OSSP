#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

int main() {
    int fd[2];
    pid_t pid;
    int data[5] = {10, 20, 30, 40, 50};

    struct timeval start, end;

    // Create pipe
    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    // Start timer
    gettimeofday(&start, NULL);

    // Create child process
    pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    // Parent - Producer
    if (pid > 0) {
        close(fd[0]);   // Close read end

        printf("Producer (Parent): Sending data...\n");

        for (int i = 0; i < 5; i++) {
            write(fd[1], &data[i], sizeof(int));
            printf("Produced: %d\n", data[i]);
        }

        close(fd[1]);

        wait(NULL);

        // Stop timer
        gettimeofday(&end, NULL);

        double time_taken =
            (end.tv_sec - start.tv_sec) * 1000000.0 +
            (end.tv_usec - start.tv_usec);

        printf("\nCommunication time: %.2f microseconds\n",
               time_taken);
    }

    // Child - Consumer
    else {
        close(fd[1]);   // Close write end

        int value;

        printf("\nConsumer (Child): Receiving data...\n");

        while (read(fd[0], &value, sizeof(int)) > 0) {
            printf("Consumed: %d\n", value);
        }

        close(fd[0]);
        exit(0);
    }

    return 0;
}

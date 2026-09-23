#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define NUM_CHILDREN 3

void child_task(int id, int sleep_time) {
    printf("[Child %d | PID: %d] Started. Working for %d seconds...\n", id, getpid(), sleep_time);
    sleep(sleep_time);
    printf("[Child %d | PID: %d] Completed work.\n", id, getpid());
    exit(10 + id); // Exit with custom status code
}

int main() {
    pid_t pids[NUM_CHILDREN];
    int sleep_times[NUM_CHILDREN] = {2, 4, 1};

    printf("--- Parent PID %d starting process spawning ---\n", getpid());

    // Spawn child processes
    for (int i = 0; i < NUM_CHILDREN; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0) {
            // Child execution
            child_task(i + 1, sleep_times[i]);
        }
    }

    // Monitor children non-blockingly using WNOHANG
    int active_children = NUM_CHILDREN;
    int status;

    printf("\n--- Parent Monitoring Child Processes ---\n");
    while (active_children > 0) {
        for (int i = 0; i < NUM_CHILDREN; i++) {
            if (pids[i] != 0) { // If child hasn't been reaped yet
                pid_t result = waitpid(pids[i], &status, WNOHANG);

                if (result == 0) {
                    // Process is still running
                    printf("[Parent] Polling: Child %d (PID %d) is still running...\n", i + 1, pids[i]);
                } else if (result == pids[i]) {
                    // Process finished
                    if (WIFEXITED(status)) {
                        printf("[Parent SUCCESS] Child %d (PID %d) exited with status: %d\n", 
                               i + 1, pids[i], WEXITSTATUS(status));
                    } else if (WIFSIGNALED(status)) {
                        printf("[Parent ALERT] Child %d (PID %d) terminated by signal: %d\n", 
                               i + 1, pids[i], WTERMSIG(status));
                    }
                    pids[i] = 0; // Mark as reaped
                    active_children--;
                } else if (result == -1) {
                    perror("waitpid error");
                    pids[i] = 0;
                    active_children--;
                }
            }
        }
        sleep(1); // Poll interval
    }

    printf("\n--- All child processes synchronized and reaped successfully ---\n");
    return 0;
}

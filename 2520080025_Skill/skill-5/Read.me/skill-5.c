#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Global variables
int global_initialized = 42;    // Initialized Data segment (.data)
int global_uninitialized;       // Uninitialized Data segment (.bss)

// Helper function to print segment addresses
void show_memory_layout(void) {
    // Local static variables
    static int static_initialized = 100; // Data segment (.data)
    static int static_uninitialized;      // BSS segment (.bss)

    // Stack variable
    int stack_variable = 10;

    // Heap variable
    int *heap_variable = (int *)malloc(sizeof(int));
    if (!heap_variable) {
        perror("malloc failed");
        return;
    }

    // Safely assign function address to a generic pointer
    void (*func_ptr)(void) = show_memory_layout;

    printf("================ Memory Address Analysis ================\n");
    printf("Process PID          : %d\n\n", getpid());

    printf("Code (Text) Segment  :\n");
    printf("  Function Address   : %p\n\n", (void *)func_ptr);

    printf("Data Segment (Initialized):\n");
    printf("  Global variable    : %p\n", (void *)&global_initialized);
    printf("  Static variable    : %p\n\n", (void *)&static_initialized);

    printf("BSS Segment (Uninitialized):\n");
    printf("  Global variable    : %p\n", (void *)&global_uninitialized);
    printf("  Static variable    : %p\n\n", (void *)&static_uninitialized);

    printf("Heap Segment         :\n");
    printf("  Allocated address  : %p\n\n", (void *)heap_variable);

    printf("Stack Segment        :\n");
    printf("  Local variable     : %p\n", (void *)&stack_variable);
    printf("=========================================================\n");

    free(heap_variable);
}

int main(void) {
    show_memory_layout();

    printf("\nProcess paused. Open a second terminal to inspect maps:\n");
    printf("  cat /proc/%d/maps\n", getpid());
    printf("\nPress ENTER to end program...");
    getchar();

    return 0;
}

#include <stdio.h>
#include <stdlib.h>

int main() {
    int n = 5;

    // 1. Using malloc()
    printf("--- 1. malloc() ---\n");
    int *arr_malloc = (int *)malloc(n * sizeof(int));
    if (arr_malloc == NULL) {
        perror("malloc failed");
        return 1;
    }
    for (int i = 0; i < n; i++) {
        arr_malloc[i] = (i + 1) * 10;
        printf("malloc_arr[%d] = %d\n", i, arr_malloc[i]);
    }

    // 2. Using calloc() - Zero-initialized
    printf("\n--- 2. calloc() ---\n");
    int *arr_calloc = (int *)calloc(n, sizeof(int));
    if (arr_calloc == NULL) {
        perror("calloc failed");
        free(arr_malloc);
        return 1;
    }
    for (int i = 0; i < n; i++) {
        printf("calloc_arr[%d] (initial) = %d\n", i, arr_calloc[i]);
    }

    // 3. Using realloc() - Resize malloc array from 5 to 8 elements
    printf("\n--- 3. realloc() ---\n");
    int new_n = 8;
    int *temp = (int *)realloc(arr_malloc, new_n * sizeof(int));
    if (temp == NULL) {
        perror("realloc failed");
        free(arr_malloc);
        free(arr_calloc);
        return 1;
    }
    arr_malloc = temp; // Assign back reallocated pointer

    for (int i = n; i < new_n; i++) {
        arr_malloc[i] = (i + 1) * 10;
    }

    printf("Resized array content:\n");
    for (int i = 0; i < new_n; i++) {
        printf("arr_malloc[%d] = %d\n", i, arr_malloc[i]);
    }

    // 4. Using free() to prevent memory leaks
    printf("\n--- 4. Freeing Memory ---\n");
    free(arr_malloc);
    free(arr_calloc);
    printf("Memory freed successfully.\n");

    /* 
     * UNCOMMENT THE LINE BELOW TO SIMULATE A MEMORY LEAK FOR VALGRIND:
     * int *leak = (int *)malloc(100 * sizeof(int));
     */

    return 0;
}

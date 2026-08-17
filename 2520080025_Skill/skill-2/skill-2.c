#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 100

/* Function declarations */
void displayPrompt(void);
void processCommand(char *command);
void clearInputBuffer(char *buffer, int *position);

int main(void)
{
    char inputBuffer[BUFFER_SIZE];
    int position = 0;
    int ch;

    printf("=====================================\n");
    printf("       INTERACTIVE COMMAND LOOP      \n");
    printf("=====================================\n");
    printf("Type a command and press Enter.\n");
    printf("Available commands:\n");
    printf("  help  - Display available commands\n");
    printf("  hello - Display greeting\n");
    printf("  clear - Clear the screen\n");
    printf("  exit  - Exit the program\n");
    printf("=====================================\n\n");

    /* Main Loop */
    while (1)
    {
        displayPrompt();

        position = 0;
        memset(inputBuffer, 0, sizeof(inputBuffer));

        /* Capture keyboard input */
        while (1)
        {
            ch = getchar();

            /* Handle Enter key */
            if (ch == '\n' || ch == '\r')
            {
                inputBuffer[position] = '\0';
                printf("\n");

                /* Process the command */
                processCommand(inputBuffer);

                /* Exit condition */
                if (strcmp(inputBuffer, "exit") == 0)
                {
                    printf("Program terminated successfully.\n");
                    return 0;
                }

                break;
            }

            /* Handle Backspace */
            else if (ch == '\b' || ch == 127)
            {
                if (position > 0)
                {
                    position--;

                    inputBuffer[position] = '\0';

                    /*
                     * Move cursor back, erase character,
                     * and move cursor back again.
                     */
                    printf("\b \b");
                    fflush(stdout);
                }
            }

            /* Handle normal keyboard characters */
            else if (ch >= 32 && ch <= 126)
            {
                if (position < BUFFER_SIZE - 1)
                {
                    inputBuffer[position] = (char)ch;
                    position++;

                    /* Display typed character */
                    putchar(ch);
                    fflush(stdout);
                }
                else
                {
                    printf("\nInput buffer is full!\n");
                    break;
                }
            }
        }
    }

    return 0;
}


/*
 * Display command prompt
 */
void displayPrompt(void)
{
    printf("Command> ");
    fflush(stdout);
}


/*
 * Process user command
 */
void processCommand(char *command)
{
    if (strlen(command) == 0)
    {
        printf("No command entered.\n");
    }

    else if (strcmp(command, "help") == 0)
    {
        printf("\nAvailable Commands:\n");
        printf("-------------------\n");
        printf("help  - Display available commands\n");
        printf("hello - Display greeting message\n");
        printf("clear - Clear the screen\n");
        printf("exit  - Exit the program\n\n");
    }

    else if (strcmp(command, "hello") == 0)
    {
        printf("Hello! Welcome to the interactive command system.\n\n");
    }

    else if (strcmp(command, "clear") == 0)
    {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    else
    {
        printf("Unknown command: %s\n", command);
        printf("Type 'help' to see available commands.\n\n");
    }
}


/*
 * Clear input buffer
 */
void clearInputBuffer(char *buffer, int *position)
{
    memset(buffer, 0, BUFFER_SIZE);
    *position = 0;
}

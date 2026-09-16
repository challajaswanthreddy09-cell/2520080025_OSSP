#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 500
#define MAX_TOKENS 100
#define MAX_TOKEN_SIZE 50

/* ---------------- TOKEN STRUCTURE ---------------- */

typedef enum {
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_REDIRECT_APPEND,
    TOKEN_END
} TokenType;

typedef struct {
    TokenType type;
    char value[MAX_TOKEN_SIZE];
} Token;


/* ---------------- TOKEN TYPE NAME ---------------- */

const char *tokenName(TokenType type)
{
    switch (type) {
        case TOKEN_WORD:
            return "WORD";
        case TOKEN_PIPE:
            return "PIPE";
        case TOKEN_REDIRECT_IN:
            return "REDIRECT_IN";
        case TOKEN_REDIRECT_OUT:
            return "REDIRECT_OUT";
        case TOKEN_REDIRECT_APPEND:
            return "REDIRECT_APPEND";
        case TOKEN_END:
            return "END";
        default:
            return "UNKNOWN";
    }
}


/* ---------------- ADD TOKEN ---------------- */

void addToken(Token tokens[], int *count, TokenType type, const char *value)
{
    if (*count >= MAX_TOKENS - 1)
        return;

    tokens[*count].type = type;

    if (value != NULL)
        strncpy(tokens[*count].value, value, MAX_TOKEN_SIZE - 1);
    else
        tokens[*count].value[0] = '\0';

    tokens[*count].value[MAX_TOKEN_SIZE - 1] = '\0';

    (*count)++;
}


/* ---------------- LEXER ---------------- */

int tokenize(char *input, Token tokens[])
{
    int i = 0;
    int count = 0;

    while (input[i] != '\0') {

        /* Handle whitespace */
        if (isspace(input[i])) {
            i++;
            continue;
        }

        /* Handle pipe */
        if (input[i] == '|') {
            addToken(tokens, &count, TOKEN_PIPE, "|");
            i++;
            continue;
        }

        /* Handle input redirection */
        if (input[i] == '<') {
            addToken(tokens, &count, TOKEN_REDIRECT_IN, "<");
            i++;
            continue;
        }

        /* Handle output redirection */
        if (input[i] == '>') {

            if (input[i + 1] == '>') {
                addToken(tokens, &count,
                         TOKEN_REDIRECT_APPEND, ">>");
                i += 2;
            }
            else {
                addToken(tokens, &count,
                         TOKEN_REDIRECT_OUT, ">");
                i++;
            }

            continue;
        }

        /* Handle words */
        if (isalnum(input[i]) || input[i] == '_' ||
            input[i] == '-' || input[i] == '.') {

            char word[MAX_TOKEN_SIZE];
            int j = 0;

            while (input[i] != '\0' &&
                   !isspace(input[i]) &&
                   input[i] != '|' &&
                   input[i] != '<' &&
                   input[i] != '>') {

                if (j < MAX_TOKEN_SIZE - 1)
                    word[j++] = input[i];

                i++;
            }

            word[j] = '\0';

            addToken(tokens, &count, TOKEN_WORD, word);
            continue;
        }

        /* Unknown character */
        printf("Lexer Error: Invalid character '%c'\n",
               input[i]);

        i++;
    }

    addToken(tokens, &count, TOKEN_END, "END");

    return count;
}


/* ---------------- DISPLAY TOKENS ---------------- */

void displayTokens(Token tokens[], int count)
{
    int i;

    printf("\n========== TOKEN STREAM ==========\n");

    for (i = 0; i < count; i++) {
        printf("Token %d : %-18s -> %s\n",
               i + 1,
               tokenName(tokens[i].type),
               tokens[i].value);
    }
}


/* ---------------- PARSER ---------------- */

int parseCommand(Token tokens[], int count)
{
    int i = 0;
    int commandHasWord = 0;

    printf("\n========== PARSER ==========\n");

    /* Empty command */
    if (tokens[0].type == TOKEN_END) {
        printf("Parser: Empty command\n");
        return 0;
    }

    while (tokens[i].type != TOKEN_END) {

        /* WORD */
        if (tokens[i].type == TOKEN_WORD) {

            printf("Execution: Command/Argument = %s\n",
                   tokens[i].value);

            commandHasWord = 1;
            i++;
            continue;
        }

        /* PIPE */
        if (tokens[i].type == TOKEN_PIPE) {

            if (!commandHasWord) {
                printf("Parser Error: Pipe cannot appear here\n");
                return 0;
            }

            if (tokens[i + 1].type == TOKEN_PIPE ||
                tokens[i + 1].type == TOKEN_END) {

                printf("Parser Error: Missing command after pipe\n");
                return 0;
            }

            printf("Execution Structure: PIPE\n");

            commandHasWord = 0;
            i++;
            continue;
        }

        /* INPUT REDIRECTION */
        if (tokens[i].type == TOKEN_REDIRECT_IN) {

            if (!commandHasWord) {
                printf("Parser Error: Invalid input redirection\n");
                return 0;
            }

            if (tokens[i + 1].type != TOKEN_WORD) {
                printf("Parser Error: Missing input filename\n");
                return 0;
            }

            printf("Execution Structure: INPUT <- %s\n",
                   tokens[i + 1].value);

            i += 2;
            continue;
        }

        /* OUTPUT REDIRECTION */
        if (tokens[i].type == TOKEN_REDIRECT_OUT) {

            if (!commandHasWord) {
                printf("Parser Error: Invalid output redirection\n");
                return 0;
            }

            if (tokens[i + 1].type != TOKEN_WORD) {
                printf("Parser Error: Missing output filename\n");
                return 0;
            }

            printf("Execution Structure: OUTPUT -> %s\n",
                   tokens[i + 1].value);

            i += 2;
            continue;
        }

        /* APPEND REDIRECTION */
        if (tokens[i].type == TOKEN_REDIRECT_APPEND) {

            if (!commandHasWord) {
                printf("Parser Error: Invalid append redirection\n");
                return 0;
            }

            if (tokens[i + 1].type != TOKEN_WORD) {
                printf("Parser Error: Missing output filename\n");
                return 0;
            }

            printf("Execution Structure: APPEND >> %s\n",
                   tokens[i + 1].value);

            i += 2;
            continue;
        }

        i++;
    }

    if (!commandHasWord) {
        printf("Parser Error: Empty command\n");
        return 0;
    }

    printf("\nSyntax Validation: SUCCESS\n");
    printf("Parsing completed successfully.\n");

    return 1;
}


/* ---------------- MAIN ---------------- */

int main()
{
    char input[MAX_INPUT];
    Token tokens[MAX_TOKENS];

    printf("====================================\n");
    printf("       SIMPLE COMMAND PARSER\n");
    printf("====================================\n");

    printf("\nEnter command: ");

    fgets(input, sizeof(input), stdin);

    /* Remove newline */
    input[strcspn(input, "\n")] = '\0';

    /* Lexical Analysis */
    int count = tokenize(input, tokens);

    /* Display Token Stream */
    displayTokens(tokens, count);

    /* Syntax Analysis / Parsing */
    parseCommand(tokens, count);

    return 0;
}

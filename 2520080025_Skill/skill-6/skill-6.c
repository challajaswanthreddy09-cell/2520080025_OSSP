#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TOKENS 64
#define MAX_TOKEN_LEN 256

// Function to parse input string while handling escape sequences and special characters
int parse_command(const char *input, char tokens[MAX_TOKENS][MAX_TOKEN_LEN]) {
    int token_count = 0;
    int char_idx = 0;
    bool in_token = false;
    bool escaped = false;

    for (int i = 0; input[i] != '\0'; i++) {
        char ch = input[i];

        if (escaped) {
            // Process escape sequences
            switch (ch) {
                case 'n': tokens[token_count][char_idx++] = '\n'; break;
                case 't': tokens[token_count][char_idx++] = '\t'; break;
                case ' ': tokens[token_count][char_idx++] = ' ';  break; // Escaped space
                case '\\': tokens[token_count][char_idx++] = '\\'; break;
                case '$': tokens[token_count][char_idx++] = '$';  break; // Escaped special symbol
                case '"': tokens[token_count][char_idx++] = '"';  break;
                case '|': tokens[token_count][char_idx++] = '|';  break;
                case '&': tokens[token_count][char_idx++] = '&';  break;
                default:
                    // Preserve literal character if unhandled escape
                    tokens[token_count][char_idx++] = ch;
                    break;
            }
            escaped = false;
        } else if (ch == '\\') {
            // Escape character encountered
            escaped = true;
            if (!in_token) {
                in_token = true;
            }
        } else if (ch == ' ' || ch == '\t') {
            // Unescaped space/tab acts as token delimiter
            if (in_token) {
                tokens[token_count][char_idx] = '\0';
                token_count++;
                char_idx = 0;
                in_token = false;
            }
        } else {
            // Regular character
            if (!in_token) {
                in_token = true;
            }
            tokens[token_count][char_idx++] = ch;
        }
    }

    // Wrap up final token if active
    if (in_token) {
        tokens[token_count][char_idx] = '\0';
        token_count++;
    }

    return token_count;
}

// Function to print and validate parser output
void validate_and_print_tokens(char tokens[MAX_TOKENS][MAX_TOKEN_LEN], int count) {
    printf("\n--- Parser Output Validation ---\n");
    printf("Total Tokens Parsed: %d\n", count);
    for (int i = 0; i < count; i++) {
        printf("Token[%d]: \"%s\"\n", i, tokens[i]);
    }
}

int main() {
    char tokens[MAX_TOKENS][MAX_TOKEN_LEN];

    // Test complex inputs containing escaped spaces, special symbols, and escaped control chars
    const char *test_input_1 = "echo Hello\\ World \\$100 \\&\\| special\\nvalue";
    const char *test_input_2 = "ls -l /path/with\\ spaced\\ directory/file.txt";

    printf("Test 1 Input: %s\n", test_input_1);
    int count1 = parse_command(test_input_1, tokens);
    validate_and_print_tokens(tokens, count1);

    printf("\n--------------------------------------------------\n");

    printf("Test 2 Input: %s\n", test_input_2);
    int count2 = parse_command(test_input_2, tokens);
    validate_and_print_tokens(tokens, count2);

    return 0;
}

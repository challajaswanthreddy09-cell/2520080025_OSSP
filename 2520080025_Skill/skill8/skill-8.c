#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_BUF_LEN 1024
#define MAX_VAR_NAME 128

// Internal symbol table structure for testing fallback variables
typedef struct {
    char name[MAX_VAR_NAME];
    char value[MAX_BUF_LEN];
} EnvironmentVar;

static EnvironmentVar custom_env[] = {
    {"USER", "alice"},
    {"ROLE", "admin"},
    {"KEY_admin", "SECRET_KEY_12345"},
    {"HOME", "/home/alice"},
    {"INNER", "ROLE"}
};
static int custom_env_count = 5;

// Function to lookup a variable value from custom environment or system environment
const char *get_var_value(const char *name) {
    // Check internal table first
    for (int i = 0; i < custom_env_count; i++) {
        if (strcmp(custom_env[i].name, name) == 0) {
            return custom_env[i].value;
        }
    }
    // Fall back to system environment variables
    const char *sys_val = getenv(name);
    if (sys_val != NULL) {
        return sys_val;
    }
    // Handle Undefined Variables: Return empty string
    return "";
}

// Function to recursively expand variables in a string
void expand_variables(const char *input, char *output, int depth) {
    if (depth > 10) { // Safety guard against infinite recursive loops
        strncpy(output, input, MAX_BUF_LEN);
        return;
    }

    char intermediate[MAX_BUF_LEN] = {0};
    int in_idx = 0;
    int out_idx = 0;
    bool expanded_any = false;

    while (input[in_idx] != '\0' && out_idx < MAX_BUF_LEN - 1) {
        // Detect Variable References using '$'
        if (input[in_idx] == '$') {
            in_idx++;
            char var_name[MAX_VAR_NAME] = {0};
            int var_idx = 0;

            if (input[in_idx] == '{') {
                // Bracketed notation ${VAR} - allows nested variables like ${KEY_${ROLE}}
                in_idx++; // Skip '{'
                int brace_depth = 1;

                while (input[in_idx] != '\0' && brace_depth > 0 && var_idx < MAX_VAR_NAME - 1) {
                    if (input[in_idx] == '{') brace_depth++;
                    else if (input[in_idx] == '}') {
                        brace_depth--;
                        if (brace_depth == 0) {
                            in_idx++; // Skip closing '}'
                            break;
                        }
                    }
                    var_name[var_idx++] = input[in_idx++];
                }
                var_name[var_idx] = '\0';

                // Support Nested Variables: Recursively expand inner contents of variable name first
                char expanded_var_name[MAX_VAR_NAME] = {0};
                expand_variables(var_name, expanded_var_name, depth + 1);

                // Fetch expanded value
                const char *val = get_var_value(expanded_var_name);
                int val_len = strlen(val);
                for (int k = 0; k < val_len && out_idx < MAX_BUF_LEN - 1; k++) {
                    intermediate[out_idx++] = val[k];
                }
                expanded_any = true;
            } else {
                // Simple notation $VAR
                while (input[in_idx] != '\0' && (isalnum(input[in_idx]) || input[in_idx] == '_') && var_idx < MAX_VAR_NAME - 1) {
                    var_name[var_idx++] = input[in_idx++];
                }
                var_name[var_idx] = '\0';

                const char *val = get_var_value(var_name);
                int val_len = strlen(val);
                for (int k = 0; k < val_len && out_idx < MAX_BUF_LEN - 1; k++) {
                    intermediate[out_idx++] = val[k];
                }
                expanded_any = true;
            }
        } else {
            intermediate[out_idx++] = input[in_idx++];
        }
    }
    intermediate[out_idx] = '\0';

    // If any nested expansions took place, run another pass to fully resolve nested variables
    if (expanded_any && strchr(intermediate, '$') != NULL) {
        expand_variables(intermediate, output, depth + 1);
    } else {
        strncpy(output, intermediate, MAX_BUF_LEN);
    }
}

// Test harness for variable expansion logic
void test_expansion(const char *test_name, const char *input) {
    char output[MAX_BUF_LEN] = {0};
    expand_variables(input, output, 0);
    printf("[%s]\n  Input : %s\n  Output: %s\n\n", test_name, input, output);
}

int main() {
    printf("=== Variable Expansion Engine ===\n\n");

    test_expansion("Test 1: Simple Variable Reference", "Hello $USER, welcome!");
    test_expansion("Test 2: Braced Variable Reference", "Home dir is ${HOME}/bin");
    test_expansion("Test 3: Undefined Variable", "Value is: '$UNDEFINED_VAR'");
    test_expansion("Test 4: Nested Variable Reference", "Key is: ${KEY_${ROLE}}");
    test_expansion("Test 5: Complex Multi-Nested Input", "User $USER (${KEY_${INNER}}) resides at ${HOME}");

    return 0;
}

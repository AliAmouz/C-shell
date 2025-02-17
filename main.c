#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Helper function to normalize spaces
char* normalize_spaces(char* str) {
    char* result = strdup(str);
    if (!result) return NULL;
    
    char* dest = result;
    char* src = str;
    int space_seen = 0;
    
    while (*src) {
        if (*src == ' ') {
            if (!space_seen) {
                *dest++ = ' ';
                space_seen = 1;
            }
        } else {
            *dest++ = *src;
            space_seen = 0;
        }
        src++;
    }
    *dest = '\0';
    
    return result;
}

char *get_path_type(const char *word) {
    char *temp_path = getenv("PATH");
    if (!temp_path) return NULL; // Check if path exists

    char *path = strdup(temp_path);  // Duplication
    if (!path) return NULL;

    char *iter_path = strtok(path, ":");
    while (iter_path) {
        DIR *dir = opendir(iter_path);
        if (!dir) { 
            iter_path = strtok(NULL, ":");
            continue;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(word, entry->d_name) == 0) {
                char *full_path = malloc(strlen(iter_path) + strlen(word) + 2);
                if (full_path) {
                    snprintf(full_path, strlen(iter_path) + strlen(word) + 2, "%s/%s", iter_path, word);
                }
                closedir(dir);
                free(path);
                return full_path; // Return full executable path
            }
        }
        closedir(dir);
        iter_path = strtok(NULL, ":");
    }

    free(path);
    return NULL;
}

// external commands
void execute_command(char **args) {
    pid_t pid = fork();
    if (pid == 0) { 
        execvp(args[0], args);
        perror("execvp"); 
        exit(1);
    } else if (pid > 0) { 
        wait(NULL);
    } else {
        perror("fork"); 
    }
}

int main() {
    while (1) {
        printf("$ ");
        fflush(stdout);

        char input[100];
        if (!fgets(input, sizeof(input), stdin)) {
            if (feof(stdin)) {
                printf("\n");
                break;  // Exit on EOF (Ctrl+D)
            }
            continue;
        }
        
        input[strcspn(input, "\n")] = 0; // Remove newline

        if (strcmp(input, "exit") == 0 || strcmp(input, "exit 0") == 0) break;

        // Skip empty input
        if (input[0] == '\0') continue;

        char *full_input = strdup(input);
        if (!full_input) {
            perror("strdup");
            continue;
        }

        char *tokenized_input = strdup(input);
        if (!tokenized_input) {
            perror("strdup");
            free(full_input);
            continue;
        }

        char *p = strtok(tokenized_input, " ");
        if (!p) {
            free(full_input);
            free(tokenized_input);
            continue;
        }

        // Built-in commands
        if (strcmp(p, "echo") == 0) {
            if (strlen(full_input) > 5) {
                char *echo_text = full_input + 5;
                while (*echo_text == ' ') echo_text++; // Skip leading spaces
                
                // Handle single quotes
                if (echo_text[0] == '\'' && echo_text[strlen(echo_text) - 1] == '\'') {
                    // Remove the surrounding single quotes
                    echo_text[strlen(echo_text) - 1] = '\0';
                    echo_text++;
                }
                
                // Normalize spaces in the output text
                char *normalized_text = normalize_spaces(echo_text);
                if (normalized_text) {
                    printf("%s\n", normalized_text);
                    free(normalized_text);
                } else {
                    printf("%s\n", echo_text);
                }
            } else {
                printf("\n");  // Just echo with no args prints a newline
            }
        } else if (strcmp(p, "type") == 0) {
            char *next = strtok(NULL, " ");
            if (!next) {
                printf("type: missing argument\n");
            } else {
                char *builtins[] = {"exit", "echo", "type", "cd", "pwd"};
                int found = 0;
                for (int i = 0; i < 5; i++) {
                    if (strcmp(next, builtins[i]) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    printf("%s is a shell builtin\n", next);
                } else {
                    char *new_status = get_path_type(next);
                    if (new_status) {
                        printf("%s is %s\n", next, new_status);
                        free(new_status);
                    } else {
                        printf("%s: not found\n", next);
                    }
                }
            }
        } else if (strcmp(p, "cd") == 0) {
            char *dir = strtok(NULL, " ");
            if (!dir || strcmp(dir, "~") == 0) {
                dir = getenv("HOME");  // Set to home directory if no argument or "~"
                if (!dir) {
                    printf("cd: HOME not set\n");
                    free(full_input);
                    free(tokenized_input);
                    continue;
                }
            }
            
            if (chdir(dir) < 0) {
                printf("cd: %s: No such file or directory\n", dir);
            }
        } else if (strcmp(p, "pwd") == 0) { 
            char cwd[1024]; // Buffer for the directory path
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("%s\n", cwd);
            } else {
                perror("getcwd"); // Print an error if getting the directory fails
            }
        } else {
            // External command execution
            char *args[10];
            int i = 0;
            args[i++] = p;
            
            char *token;
            while (i < 9 && (token = strtok(NULL, " "))) {
                args[i++] = token;
            }
            args[i] = NULL;

            char *new_status = get_path_type(args[0]);
            if (new_status) {
                execute_command(args);
                free(new_status);
            } else {
                printf("%s: command not found\n", args[0]);
            }
        }

        free(full_input);
        free(tokenized_input);
    }

    return 0;
}

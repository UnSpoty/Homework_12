// Переписать командный интерпретатор из задания по процессам так,
// чтобы он поддерживал конструкции вида:
// $ ls -la | grep a

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    char command[100];
    char *args[10]; // Array to store command arguments
    pid_t pid;

    while (1) {
        printf("CMD: ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;

        // Split the input command into arguments
        int argc = 0;
        char *token = strtok(command, " ");
        while (token != NULL) {
            args[argc] = token;
            argc++;
            token = strtok(NULL, " ");
        }
        args[argc] = NULL; // Set the NULL-terminating argument

        if (strcmp(command, "exit") == 0) {
            perror("Exit: ");
            exit(EXIT_SUCCESS);
        }

        pid = fork();

        if (pid == 0) {

            // If we are in the child process, execute the command
            if (argc > 1) {
                // If there are arguments, use execvp()
                if (execvp(args[0], args) == -1) {
                    perror("execvp");
                    exit(EXIT_FAILURE);
                }
            } else {
                // If there are no arguments, use execlp()
                if (execlp(args[0], args[0], NULL) == -1) {
                    perror("execlp");
                    exit(EXIT_FAILURE);
                }
            }
        } else {
            // If we are in the parent process, wait for the child process to finish
            if (waitpid(pid, NULL, 0) == -1) {
                perror("wait");
            }
        }

    }

    return 0;
}

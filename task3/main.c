// Переписать командный интерпретатор из задания по процессам так,
// чтобы он поддерживал конструкции вида:
// $ ls -la | grep a

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_ARGS 10

int main() {
    char command[100];
    char *args[MAX_ARGS];
    char *cmd1[MAX_ARGS];
    char *cmd2[MAX_ARGS];
    pid_t pid1, pid2;
    int pipefd[2];

    while (1) {
        printf("CMD: ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;

        // Проверка на команду выхода
        if (strcmp(command, "exit") == 0) {
            exit(EXIT_SUCCESS);
        }

        // Разбиваем команду на две части, если есть |
        char *pipe_ptr = strchr(command, '|');
        if (pipe_ptr != NULL) {
            *pipe_ptr = '\0';
            pipe_ptr++;
        }

        // Разбиваем первую команду на аргументы
        int argc1 = 0;
        char *token = strtok(command, " ");
        while (token != NULL) {
            cmd1[argc1++] = token;
            token = strtok(NULL, " ");
        }
        cmd1[argc1] = NULL;

        // Разбиваем вторую команду на аргументы, если она есть
        int argc2 = 0;
        if (pipe_ptr != NULL) {
            token = strtok(pipe_ptr, " ");
            while (token != NULL) {
                cmd2[argc2++] = token;
                token = strtok(NULL, " ");
            }
            cmd2[argc2] = NULL;
        }

        if (pipe(pipefd) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid1 = fork();
        if (pid1 == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid1 == 0) {
            // Первый дочерний процесс выполняет первую команду
            close(pipefd[0]); 
            dup2(pipefd[1], STDOUT_FILENO); // Перенаправляем stdout в pipe
            close(pipefd[1]); 

            execvp(cmd1[0], cmd1);
            perror("execvp");
            exit(EXIT_FAILURE);
        }

        pid2 = fork();
        if (pid2 == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid2 == 0) {
            // Второй дочерний процесс выполняет вторую команду
            close(pipefd[1]); 
            dup2(pipefd[0], STDIN_FILENO); // Перенаправляем stdin из pipe
            close(pipefd[0]); 

            execvp(cmd2[0], cmd2);
            perror("execvp");
            exit(EXIT_FAILURE);
        }

        // Родительский процесс
        close(pipefd[0]); 
        close(pipefd[1]);

        // Ожидаем завершения обоих дочерних процессов
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
    }

    return 0;
}


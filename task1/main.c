//Реализовать программу, которая создает неименованный канал и
//порождает процесс. Родительский процесс закрывает дескриптор для
//чтения из канала и записывает в канал строку “Hi!”, затем ожидает
//завершения дочернего процесса. Дочерний процесс закрывает
//дескриптор для записи в канал, считывает строку из канала и выводит на экран.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>  

int main() {
    int pipe_fds[2];  // File descriptors for the pipe

    // Create an unnamed pipe
    if (pipe(pipe_fds) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    int pid = fork();  // Create a child process
    if (pid < 0) {
        perror("no child process is created");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process
        close(pipe_fds[1]);  // Close the write end

        char message[1024];
        int bytes_read = 0;
        while ((bytes_read = read(pipe_fds[0], message + bytes_read, sizeof(message) - bytes_read)) > 0) {
            message[bytes_read] = '\0';  // Null-terminate the message
            printf("Message is received: %s\n", message);
            exit(EXIT_SUCCESS);
        }

        close(pipe_fds[0]);  // Close the read end
    } else {
        // Parent process
        close(pipe_fds[0]);  // Close the read end

        char message[] = "Hi!";  // Message for the child process
        if (write(pipe_fds[1], message, strlen(message) + 1) == -1) {
            perror("Error on write");
            exit(EXIT_FAILURE);
        }

        if (waitpid(pid, NULL, 0) == -1) {
            perror("Error waiting for child process");
            exit(EXIT_FAILURE);
        }

        close(pipe_fds[1]);  // Close the write end
    }

    return 0;
}

// Реализовать 2 программы, первая сервер, вторая клиент. Сервер создает
// именованный канал и открывает его только на запись, записывает строку
// “Hi!” и завершается. Клиент открывает созданный канал сервером
// только на чтение, считывает строку из канала и выводит на экран. Удает
// канал.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
    // Создание именованного канала
    const char *fifo_name = "./my_fifo";

    // Открытие канала только для чтения
    int fd = open(fifo_name, O_RDONLY);

    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Чтение сообщения из канала
    char message[1024];

    ssize_t bytes_read = read(fd, message, sizeof(message));
    if (bytes_read == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    } else if (bytes_read == 0) {
        printf("Received message: (empty)\n");
    } else {
        message[bytes_read] = '\0'; // null-terminate the string
        printf("Received message: %s\n", message);
    }

    // Закрытие канала
    close(fd);

    return 0;
}
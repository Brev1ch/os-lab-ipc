#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#define FIFO "/tmp/fifo.hello"

int main(void)
{
    int writefd;
    const char *msg = "Здравствуй, Мир!\n";
    size_t len = strlen(msg);                    /* в UTF-8 кириллица занимает 2 байта - берём реальную длину */

    if ((writefd = open(FIFO, O_WRONLY)) < 0) { printf("Невозможно открыть FIFO\n"); exit(1); }
    if (write(writefd, msg, len) != (ssize_t)len) { printf("Ошибка записи\n"); exit(1); }
    close(writefd);
    if (unlink(FIFO) < 0) { printf("Невозможно удалить FIFO\n"); exit(1); }
    return 0;
}

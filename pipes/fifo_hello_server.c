/* FIFO, уровень 1 (B): сервер создаёт FIFO и печатает то, что пришло */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO "/tmp/fifo.hello"
#define MAXBUFF 80

int main(void)
{
    int readfd, n;
    char buff[MAXBUFF];

    unlink(FIFO);                                /* на случай остатков от прошлого запуска */
    if (mkfifo(FIFO, 0666) < 0) { printf("Невозможно создать FIFO\n"); exit(1); }
    if ((readfd = open(FIFO, O_RDONLY)) < 0) { printf("Невозможно открыть FIFO\n"); exit(1); }

    while ((n = read(readfd, buff, MAXBUFF)) > 0)
        if (write(1, buff, n) != n) { printf("Ошибка вывода\n"); exit(1); }

    close(readfd);                               /* удаление FIFO - дело клиента */
    return 0;
}

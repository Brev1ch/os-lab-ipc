/* FIFO, уровень 2 (B): сервер из задания 2,3 (A) для неродственных процессов */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"
#define MAXLINE 4096

static void server(int readfd, int writefd)
{
    char buff[MAXLINE + 1];
    ssize_t n;
    int fd;

    if ((n = read(readfd, buff, MAXLINE)) == 0) {
        printf("Server: end-of-file while reading pathname\n");
        return;
    }
    buff[n] = '\0';
    printf("Server: запрошен файл '%s'\n", buff);

    if ((fd = open(buff, O_RDONLY)) < 0) {
        char err[MAXLINE + 64];
        n = snprintf(err, sizeof err, "%s: can't open, %s\n", buff, strerror(errno));
        write(writefd, err, n);
    } else {
        while ((n = read(fd, buff, MAXLINE)) > 0)
            write(writefd, buff, n);
        close(fd);
    }
}

int main(void)
{
    int readfd, writefd;

    unlink(FIFO1); unlink(FIFO2);
    if (mkfifo(FIFO1, 0666) < 0) { printf("Server: can not create FIFO1\n"); exit(1); }
    printf("Server: FIFO1 is created!\n");
    if (mkfifo(FIFO2, 0666) < 0) { unlink(FIFO1); printf("Server: can not create FIFO2\n"); exit(1); }
    printf("Server: FIFO2 is created!\n");

    if ((readfd = open(FIFO1, O_RDONLY)) < 0) { printf("Server: can not open FIFO1 for read\n"); exit(1); }
    printf("Server: FIFO1 is opened for read and readfd=%d\n", readfd);
    if ((writefd = open(FIFO2, O_WRONLY)) < 0) { printf("Server: can not open FIFO2 for write\n"); exit(1); }
    printf("Server: FIFO2 is opened for write and writefd=%d\n", writefd);

    server(readfd, writefd);
    close(readfd);
    close(writefd);
    return 0;
}

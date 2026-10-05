#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"
#define MAXLINE 4096

static void client(int readfd, int writefd)
{
    char buff[MAXLINE];
    size_t len;
    ssize_t n;

    printf("Client: введите имя файла: ");
    fflush(stdout);
    if (fgets(buff, MAXLINE, stdin) == NULL) return;
    len = strlen(buff);
    if (len > 0 && buff[len - 1] == '\n') len--;
    write(writefd, buff, len);

    while ((n = read(readfd, buff, MAXLINE)) > 0)
        write(STDOUT_FILENO, buff, n);
}

int main(void)
{
    int readfd, writefd;

    if ((writefd = open(FIFO1, O_WRONLY)) < 0) { printf("Client: can not open FIFO1 for write\n"); exit(1); }
    printf("Client: FIFO1 is opened for write writefd=%d\n", writefd);
    if ((readfd = open(FIFO2, O_RDONLY)) < 0) { printf("Client: can not open FIFO2 for read\n"); exit(1); }
    printf("Client: FIFO2 is opened for read readfd=%d\n", readfd);

    client(readfd, writefd);
    close(readfd);
    close(writefd);

    if (unlink(FIFO1) < 0) { printf("Client: can not delete FIFO1\n"); exit(1); }
    printf("Client: FIFO1 is deleted!\n");
    if (unlink(FIFO2) < 0) { printf("Client: can not delete FIFO2\n"); exit(1); }
    printf("Client: FIFO2 is deleted!\n");
    printf("Client is terminated!\n");
    return 0;
}

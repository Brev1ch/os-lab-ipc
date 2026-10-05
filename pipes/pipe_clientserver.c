/* Каналы, уровни 2-3 (A): клиент-сервер на двух неименованных каналах.
 * Клиент (отец): читает имя файла из stdin -> pipe1 -> сервер (сын).
 * Сервер открывает файл и шлёт содержимое (или сообщение об ошибке) в pipe2 -> клиент -> stdout. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 4096

static void client(int readfd, int writefd)
{
    char buff[MAXLINE];
    ssize_t n;
    size_t len;

    if (fgets(buff, MAXLINE, stdin) == NULL) { close(writefd); return; }
    len = strlen(buff);
    if (len > 0 && buff[len - 1] == '\n')
        len--;                                  /* убрать перевод строки */
    write(writefd, buff, len);
    close(writefd);                             /* сигнал серверу: запрос окончен */

    while ((n = read(readfd, buff, MAXLINE)) > 0)
        write(STDOUT_FILENO, buff, n);
}

static void server(int readfd, int writefd)
{
    char buff[MAXLINE + 1];
    ssize_t n;
    int fd;

    if ((n = read(readfd, buff, MAXLINE)) == 0) {
        const char *m = "end-of-file while reading pathname\n";
        write(writefd, m, strlen(m));
        return;
    }
    buff[n] = '\0';

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
    int pipe1[2], pipe2[2];
    pid_t childpid;

    if (pipe(pipe1) < 0 || pipe(pipe2) < 0) { perror("pipe"); exit(1); }

    if ((childpid = fork()) == 0) {             /* сын = сервер */
        close(pipe1[1]);
        close(pipe2[0]);
        server(pipe1[0], pipe2[1]);
        close(pipe2[1]);
        exit(0);
    }
    close(pipe1[0]);                            /* отец = клиент */
    close(pipe2[1]);
    client(pipe2[0], pipe1[1]);
    waitpid(childpid, NULL, 0);
    return 0;
}

/* FIFO, уровень 3 (B): сервер обменивается с клиентом структурированными сообщениями */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "mesg.h"

static void server(int readfd, int writefd)
{
    struct mymesg m;
    ssize_t n;

    while (mesg_recv(readfd, &m) >= 0) {           /* цикл по запросам до закрытия клиентом FIFO1 */
        if (m.mesg_type != MT_REQUEST) continue;
        m.mesg_data[m.mesg_len] = '\0';
        printf("Server: запрос на файл '%s'\n", m.mesg_data);
        fflush(stdout);

        int fd = open(m.mesg_data, O_RDONLY);
        if (fd < 0) {
            m.mesg_type = MT_ERROR;
            m.mesg_len = snprintf(m.mesg_data, MAXMESGDATA, "can't open: %s", strerror(errno));
            mesg_send(writefd, &m);
        } else {
            while ((n = read(fd, m.mesg_data, MAXMESGDATA)) > 0) {
                m.mesg_type = MT_DATA;
                m.mesg_len = n;
                mesg_send(writefd, &m);
            }
            close(fd);
        }
        m.mesg_type = MT_END;                      /* маркер конца ответа */
        m.mesg_len = 0;
        mesg_send(writefd, &m);
    }
    printf("Server: клиент закрыл FIFO, выход\n");
}

int main(void)
{
    int readfd, writefd;

    unlink(FIFO1); unlink(FIFO2);
    if (mkfifo(FIFO1, 0666) < 0) { printf("Server: can not create FIFO1\n"); exit(1); }
    printf("Server: FIFO1 is created!\n");
    if (mkfifo(FIFO2, 0666) < 0) { unlink(FIFO1); printf("Server: can not create FIFO2\n"); exit(1); }
    printf("Server: FIFO2 is created!\n");
    fflush(stdout);
    if ((readfd = open(FIFO1, O_RDONLY)) < 0) { printf("Server: can not open FIFO1 for read\n"); exit(1); }
    if ((writefd = open(FIFO2, O_WRONLY)) < 0) { printf("Server: can not open FIFO2 for write\n"); exit(1); }

    server(readfd, writefd);
    close(readfd);
    close(writefd);
    return 0;
}

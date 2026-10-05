#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "mesg.h"

static void client(int readfd, int writefd)
{
    struct mymesg m;
    char line[MAXMESGDATA];

    printf("Client: вводите имена файлов (Ctrl+D - выход)\n");
    while (fgets(line, sizeof line, stdin) != NULL) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[--len] = '\0';
        if (len == 0) continue;

        m.mesg_type = MT_REQUEST;
        m.mesg_len = len;
        memcpy(m.mesg_data, line, len);
        mesg_send(writefd, &m);

        while (mesg_recv(readfd, &m) >= 0 && m.mesg_type != MT_END) {
            if (m.mesg_type == MT_ERROR)
                printf("[ERROR] %.*s\n", (int)m.mesg_len, m.mesg_data);
            else
                fwrite(m.mesg_data, 1, m.mesg_len, stdout);
        }
    }
}

int main(void)
{
    int readfd, writefd;

    if ((writefd = open(FIFO1, O_WRONLY)) < 0) { printf("Client: can not open FIFO1 for write\n"); exit(1); }
    if ((readfd = open(FIFO2, O_RDONLY)) < 0) { printf("Client: can not open FIFO2 for read\n"); exit(1); }
    printf("Client: FIFO1 writefd=%d, FIFO2 readfd=%d\n", writefd, readfd);

    client(readfd, writefd);
    close(writefd);                                 /* сервер увидит EOF и завершится */
    close(readfd);

    unlink(FIFO1);
    unlink(FIFO2);
    printf("Client is terminated! (FIFO удалены)\n");
    return 0;
}

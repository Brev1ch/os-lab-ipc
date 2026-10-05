#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include "mesg.h"

/* read() из FIFO может вернуть меньше запрошенного - дочитываем до конца */
static ssize_t readn(int fd, void *buf, size_t n)
{
    size_t left = n;
    char *p = buf;
    while (left > 0) {
        ssize_t r = read(fd, p, left);
        if (r < 0) { if (errno == EINTR) continue; return -1; }
        if (r == 0) break;
        left -= r; p += r;
    }
    return n - left;
}

ssize_t mesg_send(int fd, struct mymesg *mptr)
{
    return write(fd, mptr, MESGHDRSIZE + mptr->mesg_len);
}

ssize_t mesg_recv(int fd, struct mymesg *mptr)
{
    ssize_t n = readn(fd, mptr, MESGHDRSIZE);
    if (n == 0) return -1;                         /* конец файла */
    if (n != (ssize_t)MESGHDRSIZE) { fprintf(stderr, "mesg_recv: error MESGHDRSIZE\n"); return -1; }
    if (mptr->mesg_len < 0 || mptr->mesg_len > MAXMESGDATA) { fprintf(stderr, "mesg_recv: bad len\n"); return -1; }
    if (mptr->mesg_len > 0 && readn(fd, mptr->mesg_data, mptr->mesg_len) != mptr->mesg_len) {
        fprintf(stderr, "mesg_recv: can not read data\n");
        return -1;
    }
    return mptr->mesg_len;
}

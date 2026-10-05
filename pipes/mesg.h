/* Структурированные сообщения поверх FIFO (уровень 3 B) */
#ifndef MESG_H
#define MESG_H
#include <stddef.h>
#include <sys/types.h>

#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"
#define MAXMESGDATA 4096

#define MT_REQUEST 1   /* клиент -> сервер: имя файла */
#define MT_DATA    2   /* сервер -> клиент: порция содержимого файла */
#define MT_ERROR   3   /* сервер -> клиент: текст ошибки */
#define MT_END     4   /* сервер -> клиент: конец ответа (mesg_len == 0) */

struct mymesg {
    long mesg_len;                 /* длина mesg_data */
    long mesg_type;                /* тип сообщения */
    char mesg_data[MAXMESGDATA + 1];
};
#define MESGHDRSIZE (2 * sizeof(long))

ssize_t mesg_send(int fd, struct mymesg *mptr);
/* возвращает: >=0 - длина данных, -1 - конец FIFO/ошибка */
ssize_t mesg_recv(int fd, struct mymesg *mptr);
#endif

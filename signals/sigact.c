/* Сигналы, задание B: надёжные сигналы через sigaction.
 * Во время работы обработчика SIGUSR1/SIGUSR2 сигнал SIGINT заблокирован (sa_mask).
 * Сборка:  gcc -o sigact sigact.c
 *          gcc -DSEND_SIGINT -o sigact_int sigact.c   (SIGINT посылается из обработчика)
 * Время "засыпания" можно изменить: -DNAP=5 (по умолчанию 60 с по заданию). */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#ifndef NAP
#define NAP 60
#endif

static void sig_handler(int sig)
{
    printf("Signal %d is coming (SIGINT заблокирован на время обработчика)\n", sig);
    fflush(stdout);
#ifdef SEND_SIGINT
    kill(getpid(), SIGINT);   /* порядок аргументов: (pid, sig) */
    printf("SIGINT отправлен себе, но он ждёт (заблокирован)\n");
    fflush(stdout);
#endif
    sleep(NAP);
    printf("Обработчик сигнала %d завершается\n", sig);
    fflush(stdout);
}

static void set_action(int sig, struct sigaction *new_action, struct sigaction *old_action)
{
    new_action->sa_handler = sig_handler;
    sigemptyset(&new_action->sa_mask);
    sigaddset(&new_action->sa_mask, SIGINT);        /* блокировка SIGINT */
    new_action->sa_flags = 0;
    if (sigaction(sig, new_action, old_action) < 0) {
        printf("Action is not setted\n");
        exit(1);
    }
    printf("Action is setted\n");
}

static void restore_action(int sig, struct sigaction *old_action)
{
    if (sigaction(sig, old_action, NULL) < 0) {
        printf("We can not restore action!\n");
        exit(1);
    }
    printf("We restored action!\n");
}

int main(void)
{
    struct sigaction new1, old1, new2, old2;
    printf("Sigact pid=%d\n", getpid());
    fflush(stdout);
    set_action(SIGUSR1, &new1, &old1);
    set_action(SIGUSR2, &new2, &old2);

    if (kill(getpid(), SIGUSR1) == -1)
        printf("Signal send with error!\n");
    else
        printf("Signal send OK!\n");

    sleep(NAP);
    restore_action(SIGUSR1, &old1);
    restore_action(SIGUSR2, &old2);
    return 0;
}

/* Сигналы, уровень 2 (A): sig_father + sig_son.
 * SIGUSR1/SIGUSR2 - свои обработчики, SIGINT - по умолчанию, SIGCHLD - игнорируется. */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static void sig_handler_usr1(int sig);
static void sig_handler_usr2(int sig);

/* В обработчике восстанавливаем диспозицию (ненадёжные сигналы сбрасывают её в SIG_DFL) */
static void restore_signals(void)
{
    signal(SIGUSR1, sig_handler_usr1);
    signal(SIGUSR2, sig_handler_usr2);
    signal(SIGINT,  SIG_DFL);
    signal(SIGCHLD, SIG_IGN);
}

static void sig_handler_usr1(int sig)
{
    printf("usr1 handler: получен сигнал %d\n", sig);
    printf(sig == SIGUSR1 ? "Our signal!\n" : "Not our signal\n");
    printf("pid=%d ppid=%d\n", getpid(), getppid());
    fflush(stdout);
    restore_signals();
}

static void sig_handler_usr2(int sig)
{
    printf("usr2 handler: получен сигнал %d\n", sig);
    printf(sig == SIGUSR2 ? "Our signal!\n" : "Not our signal\n");
    printf("pid=%d ppid=%d\n", getpid(), getppid());
    fflush(stdout);
    restore_signals();
}

int main(void)
{
    printf("sig_father is starting! pid=%d\n", getpid());
    fflush(stdout);
    restore_signals();

    if (fork() == 0) {
        execl("./sig_son", "sig_son", (char *)NULL);
        perror("execl");
        _exit(1);
    }
    for (;;)             /* ждём сигналов; можно слать из терминала: kill -s USR2 <pid> */
        pause();
}

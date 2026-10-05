/* Сигналы, уровень 1 (A): father порождает son1..son3 и посылает им SIGUSR1.
 * son1 - реакция по умолчанию (завершение), son2 - игнорирует, son3 - свой обработчик. */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define LOG "ps_log.txt"

static void son3_handler(int sig)
{
    (void)sig;
    signal(SIGUSR1, son3_handler);               /* ненадёжные сигналы: восстановить */
    const char msg[] = "son3: перехватил SIGUSR1 и обработал сам\n";
    write(1, msg, sizeof msg - 1);
}

static void run_son(int n)
{
    switch (n) {
    case 1: signal(SIGUSR1, SIG_DFL);  break;
    case 2: signal(SIGUSR1, SIG_IGN);  break;
    case 3: signal(SIGUSR1, son3_handler); break;
    }
    printf("son%d: pid=%d запущен\n", n, getpid());
    fflush(stdout);
    for (int i = 0; i < 4; i++)                  /* живём ~4 с, чтобы сигнал успел прийти */
        sleep(1);
    printf("son%d: завершился сам\n", n);
    exit(0);
}

int main(void)
{
    pid_t pid[3];
    unlink(LOG);

    for (int i = 0; i < 3; i++) {
        if ((pid[i] = fork()) < 0) { perror("fork"); exit(1); }
        if (pid[i] == 0) run_son(i + 1);
    }
    sleep(1);                                    /* дать сыновьям установить диспозицию */

    system("echo '=== ДО посылки сигналов ===' >> " LOG);
    system("ps -o pid,ppid,stat,comm | grep -E 'PID|father' >> " LOG);

    for (int i = 0; i < 3; i++) {
        printf("father: посылаю SIGUSR1 -> son%d (pid=%d)\n", i + 1, pid[i]);
        kill(pid[i], SIGUSR1);
    }
    sleep(1);

    system("echo '=== ПОСЛЕ посылки сигналов ===' >> " LOG);
    system("ps -o pid,ppid,stat,comm | grep -E 'PID|father' >> " LOG);

    for (int i = 0; i < 3; i++) {
        int st;
        waitpid(pid[i], &st, 0);
        if (WIFSIGNALED(st))
            printf("father: son%d убит сигналом %d\n", i + 1, WTERMSIG(st));
        else
            printf("father: son%d завершился с кодом %d\n", i + 1, WEXITSTATUS(st));
    }
    return 0;
}

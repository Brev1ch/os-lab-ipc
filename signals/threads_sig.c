/* Сигналы, уровень 3 (A): сигналы и нити (pthread).
 * Сборка:  gcc -pthread -DVARIANT=N -o threads_sigN threads_sig.c
 *  VARIANT=1 - SIGUSR1 с реакцией по умолчанию (убивает ВЕСЬ процесс)
 *  VARIANT=2 - свой обработчик SIGUSR1 только печатает сообщение
 *  VARIANT=3 - обработчик печатает и вызывает pthread_exit(NULL) -> гибнет только вторая нить
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#ifndef VARIANT
#define VARIANT 3
#endif

#if VARIANT >= 2
static void usr1_handler(int sig)
{
    (void)sig;
    const char msg[] = "thread2: пришёл SIGUSR1\n";
    write(1, msg, sizeof msg - 1);
#if VARIANT == 3
    pthread_exit(NULL);        /* формально не async-signal-safe, но этого требует задание */
#endif
}
#endif

static void *thread2(void *arg)
{
    (void)arg;
#if VARIANT >= 2
    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = usr1_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);
#endif
    for (int i = 1; ; i++) {
        printf("thread2: работаю, тик %d\n", i);
        fflush(stdout);
        sleep(1);
    }
    return NULL;
}

static void *thread1(void *arg)
{
    pthread_t t2 = *(pthread_t *)arg;
    sleep(3);
    printf("thread1: pthread_kill(t2, SIGUSR1)\n");
    fflush(stdout);
    pthread_kill(t2, SIGUSR1);
    sleep(2);
    printf("thread1: завершаюсь\n");
    return NULL;
}

int main(void)
{
    pthread_t t1, t2;
    printf("main: pid=%d, VARIANT=%d (kill pid извне убьёт весь процесс, а не нить)\n", getpid(), VARIANT);
    fflush(stdout);
    pthread_create(&t2, NULL, thread2, NULL);
    pthread_create(&t1, NULL, thread1, &t2);
    pthread_join(t1, NULL);
    printf("main: первая нить завершилась, выхожу\n");
    return 0;
}

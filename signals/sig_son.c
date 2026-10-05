#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main(void)
{
    printf("sig_son is starting! pid=%d ppid=%d\n", getpid(), getppid());
    if (kill(getppid(), SIGUSR1) == -1)
        printf("Send signal with Error!\n");
    else
        printf("Send signal to father successfully!\n");
    return 0;
}

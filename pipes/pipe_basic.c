/* Каналы, уровень 1 (A): сын читает input.txt и пишет в канал, отец выводит и сохраняет в output.txt */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int filedes[2];                 /* 0 - read; 1 - write */
    char temp_ch;

    if (pipe(filedes) < 0) { printf("Father can not create Pipe!\n"); exit(1); }
    printf("Father created Pipe!\n");
    fflush(stdout);                 /* иначе буфер скопируется в сына при fork и вывод задвоится */

    if (fork() == 0) {
        FILE *fin;
        char ch;
        close(filedes[0]);          /* сыну чтение не нужно */
        printf("Child is working!\n");
        fin = fopen("input.txt", "rt");
        if (!fin) { perror("input.txt"); close(filedes[1]); exit(1); }
        while (fscanf(fin, "%c", &ch) == 1)
            write(filedes[1], &ch, 1);
        fclose(fin);
        close(filedes[1]);          /* закрытие записи -> у отца read вернёт 0 */
        printf("End of Child!\n");
        exit(0);
    } else {
        FILE *fout;
        close(filedes[1]);          /* отцу запись не нужна */
        printf("Father is working again!\n");
        fout = fopen("output.txt", "wt");
        while (read(filedes[0], &temp_ch, 1) > 0) {
            printf("%c", temp_ch);
            fprintf(fout, "%c", temp_ch);
        }
        fclose(fout);
        wait(NULL);
        printf("End of Father!\n");
    }
    return 0;
}

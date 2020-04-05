#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

int main(int argc, char ** argv) {

    mkfifo("test_fifo", 666);
    printf("did fifo\n");
    if (fork() == 0) {
        printf("did konsument\n");
        execl("consumer", "1", "test_fifo", "wynik", "6", (char *) NULL);
    }
    if (fork() == 0) {
        execl("producent", "1", "test_fifo" ,"plik1", "3", NULL);
    }
    if (fork() == 0) {
        execl("producent", "1", "test_fifo" ,"plik2", "1", NULL);
    }
    if (fork() == 0) {
        execl("producent", "1", "test_fifo" ,"plik3", "7", NULL);
    }
    if (fork() == 0) {
        execl("producent", "1", "test_fifo" ,"plik4", "5", NULL);
    }
    if (fork() == 0) {
        execl("producent", "1", "test_fifo" ,"plik5", "6", NULL);
    }
    int good = 0;
    while (!good) {
        good = 1;
        for (int j = 0; j < 6; ++j) {
            if (!wait(NULL))
                good = 0;
        }
    }
    printf("did all\n");
    
}
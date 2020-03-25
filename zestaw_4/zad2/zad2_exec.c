#define __USE_POSIX
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#define SIGNAL SIGINT

void sigint(int s) {
    printf("%d\n", s);
}

int compare(char * c1, char * c2) {
    return (strcmp(c1, c2) == 0);
}


int main(int argc, char ** argv) {
    if (argc == 3) {

        if (compare(argv[0], "pending")) {
            sigset_t newmask;
            sigpending(&newmask);
            if (sigismember(&newmask, SIGNAL)) {
                printf("widoczny\n");
            }
            else {
                printf("niewidoczny\n");
            }
        }

        raise(SIGNAL);
        printf("Udalo sie zatrzymac\n");
    }
    if (compare(argv[1], "ignore")) {
        signal(SIGNAL, SIG_IGN);
    }

    if (compare(argv[1], "mask")) {
        sigset_t newmask;
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGNAL);
        sigprocmask(SIG_BLOCK, &newmask, NULL);
        sigprocmask(SIG_SETMASK, &newmask, NULL);
    }

    if (compare(argv[1], "pending")) {
        sigset_t newmask;
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGNAL);
        sigprocmask(SIG_BLOCK, &newmask, NULL);
        sigprocmask(SIG_SETMASK, &newmask, NULL);
    }

    raise(SIGNAL);

    if (compare(argv[1], "ignore")) {
        printf("bayo");
        fflush(stdout);
        sleep(1);
        execl("zad2_exec", "ignore", "0", "1", NULL);
    }

    if (compare(argv[1], "mask")) {
        printf("bayo");
        fflush(stdout);
        sleep(1);
        execl("zad2_exec", "mask", "0", "1", NULL);
    
    }
    
    if (compare(argv[1], "pending")) {
        printf("bayo");
        fflush(stdout);
        sleep(1);
        execl("zad2_exec", "pending", "0", "1", NULL);
    }
}
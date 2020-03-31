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
    printf("handler uruchomiony\n");
}

int compare(char * c1, char * c2) {
    return (strcmp(c1, c2) == 0);
}


int main(int argc, char ** argv) {
    if (argc < 2) {
        printf("wrong arguments");
        return -1;
    }
    if (argc == 3) {

        if (compare(argv[0], "pending")) {
            sigset_t newmask;
            sigpending(&newmask);
            if (sigismember(&newmask, SIGNAL)) {
                printf("Exec: sygnal jest oczekujacy\n");
            }
            else {
                printf("Exec: sygnal nie jest oczekujacy\n");
            }
        }
        else {
            raise(SIGNAL);
            printf("Exec: Sygnal SIGINT nie przerwal dzialania\n");
        }
    }
    if (compare(argv[1], "ignore")) {
        signal(SIGNAL, SIG_IGN);
    }

    if (compare(argv[1], "handler")) {
        signal(SIGNAL, sigint);
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
        execl("zad2_exec", "ignore", "0", "1", NULL);
    }

    if (compare(argv[1], "mask")) {
        execl("zad2_exec", "mask", "0", "1", NULL);
    
    }
    
    if (compare(argv[1], "pending")) {
        execl("zad2_exec", "pending", "0", "1", NULL);
    }
}
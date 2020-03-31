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
    printf("Handler uruchomiony\n");
}

int compare(char * c1, char * c2) {
    return (strcmp(c1, c2) == 0);
}


int main(int argc, char ** argv) {

    if (argc < 2) {
        printf("wrong arguments\n");
        return -1;
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
    printf("SIGINT nie zatrzymal dzialania programu\n");

    if (compare(argv[1], "ignore")) {
        pid_t pid = fork();
        if (pid == 0) {
            raise(SIGNAL);
            printf("Fork: sygnal pomyslnie ignorowany\n");
        }
    }

    if (compare(argv[1], "handler")) {
        pid_t pid = fork();
        if (pid == 0) {
            raise(SIGNAL);
        }
    }
    if (compare(argv[1], "mask")) {
        pid_t pid = fork();
        if (pid == 0) {
            raise(SIGNAL);
            printf("Fork: SIGINT nie zatrzymal dzialania programu\n");
        }
    
    }
    if (compare(argv[1], "pending")) {
        pid_t pid = fork();
        if (pid == 0) {
            sigset_t newmask;
            sigpending(&newmask);
            if (sigismember(&newmask, SIGNAL)) {
                printf("Sygnal oczekujacy widoczny\n");
            }
            else {
                printf("Sygnal oczekujacy niewidoczny\n");
            }
        }
    }

    


}
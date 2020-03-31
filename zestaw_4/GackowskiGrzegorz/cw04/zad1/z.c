#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
int waiting = 0;

void sigint(int sig){
	printf("Odebrano sygnal SIGINT\n");
	exit(0);
}

void sigstp(int sig){
    if (waiting == 1){
        waiting = 0;
    }
    else if (waiting == 0){
        printf("Oczekuje na CTRL+Z - kontynuacja albo CTR+C - zakończenie programu\n");
        waiting = 1;
    }
    
}

int main() {
    signal(SIGINT, sigint);
    struct sigaction psa;
    psa.sa_handler = sigstp;
    sigaction(SIGTSTP, &psa, NULL);
    while(1) {
        if (waiting == 0) 
            system("ls -al");
        sleep(1);

    }
}
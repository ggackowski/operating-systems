#define __USE_POSIX
#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

int usr1_cnt = 0;
int mode = -1;
int finish = 0;
int count;

int cmp(char * a, char * b) {
    return (strcmp(a, b) == 0);
}

void sig1_handler(int sig, siginfo_t * info, void * ucontext) {
    usr1_cnt++;
    if (mode == 1) {
        count = info->si_value.sival_int;
    }
}

void sig2_handler(int sig) {
    finish = 1;
}

int main(int argc, char ** argv) {
    if (argc < 4) {
        printf("too little arguments");
        return 1;
    }

    if (cmp(argv[3], "kill")) 
        mode = 0;
    else if (cmp(argv[3], "sigqueue"))
        mode = 1;
    else if (cmp(argv[3], "sigrt"))
        mode = 2;
   
    if (mode == -1) {
        printf("incorrect mode");
        return 1;
    }


    struct sigaction act;
    if (mode != 2) {
        signal(SIGUSR2, sig2_handler);
        act.sa_flags = SA_SIGINFO;
        act.sa_sigaction = sig1_handler;
        sigaction(SIGUSR1, &act, NULL);
    }
    else {
        signal(SIGRTMIN + 2, sig2_handler);
        act.sa_flags = SA_SIGINFO;
        act.sa_sigaction = sig1_handler;
        sigaction(SIGRTMIN + 1, &act, NULL);
    }

    pid_t catcher_pid = atoi(argv[1]);
    int signal_cnt = atoi(argv[2]);
    int i;
    if (mode == 0) {
        for (i = 0; i < signal_cnt; ++i)  {
            kill(catcher_pid, SIGUSR1);
        }
        kill(catcher_pid, SIGUSR2);
    }
    if (mode == 1) {
        for (i = 0; i < signal_cnt; ++i) {
            sigqueue(catcher_pid, SIGUSR1, (union sigval) 1);
        }
        sigqueue(catcher_pid, SIGUSR2, (union sigval) 1);
    }
    if (mode == 2) {
        for (i = 0; i < signal_cnt; ++i)  {
            kill(catcher_pid, SIGRTMIN + 1);
        }
        kill(catcher_pid, SIGRTMIN + 2);
    }

    while (!finish) {
        
    }
    if (mode == 1) {
        printf("[Sender] Catcher has received %d signals\n", count);
    }
    printf("[Sender] I sent %d signals, but I got back %d of them\n", signal_cnt, usr1_cnt);
    
}
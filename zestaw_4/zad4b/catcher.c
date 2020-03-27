#define __USE_POSIX
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

int usr1_cnt = 0;
int sending = 0;
int sigq = 0;
int rt = 0;
int wwait = 0;
pid_t sender_pid;

void handler_sigusr1(int sig, siginfo_t * info, void * ucontext) {
    if (info->si_value.sival_int == 1) {
        sigq  = 1;
    }
    if (sending == 0) {
    usr1_cnt++;
    sender_pid = info->si_pid;
    kill(sender_pid, SIGUSR1);
    }
    else {
       wwait = 0;
    }
}

void handler_sigusr2(int sig) {
    sending = 1;
}

void handler_sigrt1(int sig, siginfo_t * info, void * ucontext) {
    rt = 1;
    usr1_cnt++;
    sender_pid = info->si_pid;

}

int main(int argc, char ** argv) {
    printf("pid: %d\n", getpid());
    signal(SIGUSR2, handler_sigusr2);
    signal(SIGRTMIN + 2, handler_sigusr2);

    struct sigaction act1;

    act1.sa_flags = SA_SIGINFO;
    act1.sa_sigaction = handler_sigusr1;
    sigaction(SIGUSR1, &act1, NULL);
    struct sigaction act2;

    act2.sa_flags = SA_SIGINFO;
    act2.sa_sigaction = handler_sigrt1;
    sigaction(SIGRTMIN + 1, &act2, NULL);


    while (1) {
        if (sending) {
            if (rt == 1) {
                for (int i = 0; i < usr1_cnt; ++i) {
                    while(wwait);
                    kill(sender_pid, SIGRTMIN + 1);
                    wwait = 1;
                }
                kill(sender_pid, SIGRTMIN + 2);
                printf("[Catcher] I got and sent %d SIGUSR1\n", usr1_cnt);
                return 0;
            }
            else {
                for (int i = 0; i < usr1_cnt; ++i) {
                 if (sigq == 1) {
                    sigqueue(sender_pid, SIGUSR1, (union sigval) (i + 1));
                }
                else {
                    while (wwait);
                    kill(sender_pid, SIGUSR1);
                    wwait = 1;

                }
            }
            kill(sender_pid, SIGUSR2);
            printf("[Catcher] I got and sent %d SIGUSR1\n", usr1_cnt);
            return 0;
            }
        }
    }
}
#define __USE_POSIX
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

void handler_sigusr1(int sig, siginfo_t * info, void * ucontext) {
    printf("SA_SIGINFO flag is turned on\n");
    printf("%d\n", info->si_uid);
}

int main(int argc, char ** argv) {
    printf("pid: %d\n", getpid());
    
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handler_sigusr1;
    sigaction(SIGUSR1, &act, NULL);
}
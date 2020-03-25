#define __USE_POSIX
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

void handler_siginfo(int sig, siginfo_t * info, void * ucontext) {
    printf("SA_SIGINFO flag is turned on\n");
    printf("%d\n", info->si_uid);
}


int main() {

    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handler_siginfo;
    sigaction(SIGINT, &act, NULL);

    while (1) {
        printf("a");
        fflush(stdout);
        sleep(1);
    }


}

//omijając numer sygnału oraz identyfikator PID procesu wysyłającego)
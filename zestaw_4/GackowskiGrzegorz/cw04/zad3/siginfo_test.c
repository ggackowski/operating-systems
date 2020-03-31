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
    printf("\nSA_SIGINFO flag is turned on\n");
    printf("Wartosc mozliwa do wyslania przez sigqueue %d\n (w celu bardziej zaawansowanej komunikacji pomiedzy procesami\n", info->si_value.sival_int); 
    printf("User ID wysylajacego sygnal procesu %d\n", info->si_uid);
    printf("Czas uzytkownika %ld\n\n", info->si_utime);
}


int main() {

    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handler_siginfo;
    sigaction(SIGINT, &act, NULL);
    int cnt = 0;

    while (1) {
        printf("a");
        fflush(stdout);
        if (cnt == 5) {
        sigqueue(getpid(), SIGINT, (union sigval) 6);
        }
        cnt++;
        sleep(1);
    }


}

//omijając numer sygnału oraz identyfikator PID procesu wysyłającego)
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

int usr1_cnt = 0;
int mode = -1;
int finish = 0;

int cmp(char * a, char * b) {
    return (strcmp(a, b) == 0);
}

void usr1_handler(int sig) {
    usr1_cnt++;
}

void usr2_handler(int sig) {
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


    signal(SIGUSR2, usr2_handler);
    signal(SIGUSR1, usr1_handler);

    pid_t catcher_pid = atoi(argv[1]);
    int signal_cnt = atoi(argv[2]);
    int i;
    if (mode == 0) {
        for (i = 0; i < signal_cnt; ++i) 
            kill(catcher_pid, SIGUSR1);
        kill(catcher_pid, SIGUSR2);
    }

    while (!finish) {
        
    }

    printf("I've recreived !! %d !! signals, and I should have received %d signals\n", usr1_cnt, signal_cnt);
    
}
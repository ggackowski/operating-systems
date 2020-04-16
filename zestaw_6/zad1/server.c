#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char ** argv) {
    key_t key = ftok("$HOME", 'c');
    int queue = msgget(key, IPC_CREAT);
    
    struct msgbuf message;
    message.mtype = 0;
    message.mtext = calloc(100, sizeof(char));
    strcpy(message.mtext, "test komunikatu");

    msgsnd(queue, message, 100 * sizeof(char), 0);

    struct msgbuf response;
    response.mtype = 0;
    response.mtext = calloc(100, sizeof(char));    
    msgrcv(queue, response, 100 * sizeof(char), 0, 0);

    printf("%s\n", response.mtext);

}
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
#include <errno.h>
typedef struct {
    long mtype;
    char mtext[100];
} msgbuf;

int show_err(char * where) {
    if (errno) {
        printf("%s: %s\n", where, strerror(errno));
        return 1;
    }
    return 0;
}

int main(int argc, char ** argv) {
    key_t key = ftok(getenv("HOME"), 1);
    
    show_err("Key create error");
    
    int queue = msgget(key, IPC_CREAT | 0777);
    
    show_err("Queue make error");
    

    msgbuf message;
    message.mtype = 1;
    strcpy(message.mtext, "test komunikatu");
    
    msgsnd(queue, &message, sizeof(message.mtext), IPC_NOWAIT);

    show_err("message send error");


    msgbuf response;
    response.mtype = 2;
    
    msgrcv(queue, &response, sizeof(response.mtext), 0, MSG_NOERROR | IPC_NOWAIT);
    
    show_err("message receive error");
        
    printf("%s\n", response.mtext);
    
    msgctl(queue, IPC_RMID, NULL);

}
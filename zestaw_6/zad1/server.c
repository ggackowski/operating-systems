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

void send_mess(int queue, char * message) {
    msgbuf mesbuf;
    mesbuf.mtype = 1;
    strcpy(mesbuf.mtext, message);
    msgsnd(queue, &mesbuf, sizeof(mesbuf.mtext), IPC_NOWAIT);
}

char * get_mess(int queue) {
    msgbuf * response = calloc(1, sizeof(msgbuf));
    response->mtype = 1;
    msgrcv(queue, response, sizeof(response->mtext), 0, MSG_NOERROR | IPC_NOWAIT);
    return response->mtext;
}

int main(int argc, char ** argv) {
    key_t key = ftok(getenv("HOME"), 1411);
    
    show_err("Key create error");
    
    int queue = msgget(key, IPC_CREAT | 0777);
    
    show_err("Queue make error");
    

    send_mess(queue, "Komunikati");

    show_err("message send error");


    char * res = get_mess(queue);
    
    show_err("message receive error");
        
    printf("%s\n", res);
    
    msgctl(queue, IPC_RMID, NULL);

}
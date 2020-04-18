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
#include "definitions.h"

int server_queue;
int users[MAX_CLIENTS];
int talks[MAX_CLIENTS];
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

char * string(int i) {
    char * buff = calloc(100, sizeof(char));
    sprintf(buff, "%d", i);
    return buff;
}

void send_mess(int queue, char * message, int mess_type) {
    msgbuf mesbuf;
    mesbuf.mtype = mess_type;
    strcpy(mesbuf.mtext, message);
    msgsnd(queue, &mesbuf, sizeof(mesbuf.mtext), 0);
}

msgbuf * get_mess(int queue) {
    msgbuf * response = calloc(1, sizeof(msgbuf));
    response->mtype = 1;
    msgrcv(queue, response, sizeof(response->mtext), 0, MSG_NOERROR);
    return response;
}

void sigint(int s) {
    char task[10];
    strcpy(task, "task");
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (users[i] != 0) {
            send_mess(users[i], task, DISCONNECT);
        }
    }
    msgctl(server_queue, IPC_RMID, NULL);
    exit(0);
}

char * get_named_mess(char * mess, int * _id) {
    char delim[2] = "|";
    char * tmp = calloc(20, sizeof(char));
    tmp = strtok(mess, delim);
    *_id = atoi(tmp);
    tmp = strtok(NULL, mess);
    strcpy(mess, tmp);
    return mess;
}

int main(int argc, char ** argv) {

    signal(SIGINT, sigint);

    
    for (int i = 0; i < MAX_CLIENTS; ++i)  {users[i] = 0; talks[i] = 0; }


    char * buff;
    char num[100];
    key_t server_key = ftok(getenv("HOME"), SERVER_ID);
    if (show_err("Key create error")) return -1;

    printf("Server Key: %d\n", server_key);
    
    server_queue = msgget(server_key, IPC_CREAT | 0777);
    if (show_err("Queue make error")) return -1;


    msgbuf * response; 
        
  
    int client_id = -1;
    int chat_id = -1;
    while (1) {

        response = get_mess(server_queue);
        if (errno == NO_MESSAGES);
        else if (show_err("message receive error")) return -1;
        else {
            
            printf("%s\n", response->mtext);
            switch (response->mtype) {
                case INIT:
                    for (int i = 0; i < MAX_CLIENTS; ++i) {
                        if (users[i] == 0) {
                            users[i] = msgget(atoi(response->mtext), IPC_CREAT | 0777);
                            if (show_err("users i kolejak")) return -1;
                            char mess[100];
                            strcpy(mess, string(i));
                            //printf("%s\n", mess);
                            send_mess(users[i], mess, 1);
                            if (show_err("przesyl err")) return -1;
                            break;
                        }
                    }
                    //printf("got init\n");
                    break;

                case LIST:
                    get_named_mess(response->mtext, &client_id);
                    buff = calloc(1024, sizeof(char));
                    for (int i = 0; i < MAX_CLIENTS; ++i) {
                        if (users[i] != 0) {
                            if (i != client_id && talks[i] != 1)
                                sprintf(num, "%d: 1 ", i);
                            else sprintf(num, "%d: 0 ", i);
                            strcat(buff, num);
                        }
                    }
                    send_mess(users[client_id], buff, 1);
                    break;

                case CONNECT:
               
                    get_named_mess(response->mtext, &client_id);
                    chat_id = atoi(response->mtext);
                    //printf("konekts %d with %d\n", client_id, chat_id);
                    if (client_id == chat_id) {
                       
                    }
                    else if (users[chat_id] && !talks[chat_id]) {
                        printf("convo started\n");
                        sprintf(num, "%d", users[chat_id]);
                        send_mess(users[client_id], num, CONNECT);
                        sprintf(num, "%d", users[client_id]);
                        send_mess(users[chat_id], num, CONNECT);
                        talks[chat_id] = 1;
                        talks[client_id] = 1; 
                    }
                    break;

                case DISCONNECT:
                    
                    get_named_mess(response->mtext, &client_id);
                    printf("%d disconnected\n", client_id);
                    talks[client_id] = 0;
                    break;

                case STOP:

                    get_named_mess(response->mtext, &client_id);
                    printf("user %d has logged out\n", client_id); 
                    users[client_id] = 0;
                    talks[client_id] = 0;
                    break;

                default:
                    printf("err");
                    break;
            }
        }
    }

    

}
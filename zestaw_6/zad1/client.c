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
int chat_id;
int queue;
int id = -1;
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

void send_mess(int queue, char * message, int mess_type) {
    msgbuf mesbuf;
    mesbuf.mtype = mess_type;
    strcpy(mesbuf.mtext, message);
    msgsnd(queue, &mesbuf, sizeof(mesbuf.mtext), 0);
}

void send_named_mess(int queue, char * message, int mess_type) {
    char * m = calloc(100, sizeof(char));
    sprintf(m, "%d|%s", id, message);
    send_mess(queue, m, mess_type);
}

void sigint(int s) {
    char task[10];
    strcpy(task, "stop");
    send_mess(chat_id, task, DISCONNECT);
    send_named_mess(server_queue, task, STOP);
    msgctl(queue, IPC_RMID, NULL);
    exit(0);
}

msgbuf * get_mess(int queue) {
    msgbuf * response = calloc(1, sizeof(msgbuf));
    response->mtype = 1;
    msgrcv(queue, response, sizeof(response->mtext), 0, MSG_NOERROR);
    return response;
}

void chat() {

    struct msqid_ds buf;
    int num_messages;
    
    printf("polaczony %d z %d\n", queue, chat_id);
    printf("WELCOME\n\n");
    msgbuf * msg;
    int enter = 0;
    int end = 0;

    do {
        
        msgctl(queue, IPC_STAT, &buf);
        num_messages = buf.msg_qnum;
        
        if (num_messages > 0)
            printf(":: ");

        while (num_messages > 0) {
            enter = 0;
            msg = get_mess(queue);
            if (msg->mtype == DISCONNECT) {
                send_mess(chat_id, msg->mtext, DISCONNECT);
                end = 1;
                break;
            }
            msgctl(queue, IPC_STAT, &buf);
            num_messages = buf.msg_qnum;
            printf("%s ", msg->mtext);

        }
        if (!end) {

            if (enter == 0) {
                printf("\n");
                enter = 1;
            }

            char txt[100];
        
            scanf("%s", txt);

            if (strcmp(txt, "\\?"))
                send_mess(chat_id, txt, CONNECT);
        }
        

    } while (!end);

}

int main(int argc, char ** argv) {

    srand(time(NULL));
    signal(SIGINT, sigint);

    struct msqid_ds buf;
    int num_messages;

    key_t client_key = ftok(getenv("HOME"), rand() % (MAX_CLIENTS * 10));
    if (show_err("Key create error")) return -1;

    queue = msgget(client_key, IPC_CREAT | 0777);
    if (show_err("Queue make error")) return -1;
    
    int server_key = atoi(argv[1]);
  
    server_queue = msgget(server_key, IPC_CREAT | 0777);
    if (show_err("Error opening server queue")) return -1;

    char queue_key[100];
    sprintf(queue_key, "%d", client_key);

    send_mess(server_queue, queue_key, INIT);
    if (show_err("Error sending message to server queue")) return -1;
    int ready = 0;
    char additional[20];
    char task[20];
    //int fk = 0;

    while (1) {
        //sleep(1);
        //printf("next\n");
        if (id != -1) {
            ready = 1;
        }
        if (ready) {
        
                printf("client %d $ ", id);
                scanf("%s", task);
                
               
            
            if (0 == strcmp(task, "CONNECT")) {
                scanf("%s", additional);
                send_named_mess(server_queue, additional, CONNECT);
            }
            else if (0 == strcmp(task, "LIST")) {
                send_named_mess(server_queue, task, LIST);
            }
            else if (0 == strcmp(task, "DISCONNECT")) {
                send_named_mess(server_queue, task, DISCONNECT);
            }
            else if (0 == strcmp(task, "STOP")) {
               // sigint(0);
                send_named_mess(server_queue, task, STOP);
                msgctl(queue, IPC_RMID, NULL);
                exit(0);
            }
            else {
                //continue;
            }
            

        }
        sleep(1);
        msgctl(queue, IPC_STAT, &buf);
        num_messages = buf.msg_qnum;
        while (num_messages > 0) {
            //printf("nm: %d\n", num_messages);
            msgbuf * response;
            response = get_mess(queue);
            msgctl(queue, IPC_STAT, &buf);
            num_messages = buf.msg_qnum;
            //printf("r: %s\n", response->mtext);
                        
             if (!ready) {
                  id = atoi(response->mtext); 
                  printf("id %d\n", id);               
              }
             if (ready) {
                  printf("\n%s\n", response->mtext);
                    if (response->mtype == CONNECT) {
                        chat_id = atoi(response->mtext);
                        chat();
                    }
                }
        }
        
        
    }

    
    msgctl(queue, IPC_RMID, NULL);
    kill(0, SIGTERM);
}
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
#include <mqueue.h>

mqd_t server_queue;
mqd_t chat_id;
mqd_t queue;

char * queue_name;

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

void send_mess(mqd_t queue, char * message, int mess_type) {
    char mess[MESSAGE_LEN];
    sprintf(mess, "%c%s", (char)mess_type, message);
    mq_send(queue, mess, MESSAGE_LEN, mess_type);
}

void send_named_mess(mqd_t queue, char * message, int mess_type) {
    char * m = calloc(100, sizeof(char));
    sprintf(m, "%d|%s", id, message);
    send_mess(queue, m, mess_type);
}

void sigint(int s) {
    char task[10];
    strcpy(task, "stop");
    send_mess(chat_id, task, DISCONNECT);
    send_named_mess(server_queue, task, STOP);
    mq_close(queue);
    mq_unlink(queue_name);
    exit(0);
}

msgbuf * get_mess(mqd_t queue) {
    msgbuf * response = calloc(1, sizeof(msgbuf));
    char receive[MESSAGE_LEN + 10];
    unsigned pri;
    mq_receive(queue, receive, MESSAGE_LEN + 10, &pri);
    response->mtype = receive[0];
    sprintf(response->mtext, "%s", receive + 1);

    return response;
}

int has_messages(mqd_t queue) {
    struct mq_attr queue_info;
    mq_getattr(queue, &queue_info);
    return queue_info.mq_curmsgs;
}

void chat() {



    
    printf("Now chatting \n[write \\end to end conversation]\n");

    msgbuf * msg;
    int enter = 0;
    int end = 0;
    char txt[100];
    do {
        
        
        if (has_messages(queue))
            printf(":: ");

        while (has_messages(queue)) {
            enter = 0;
            msg = get_mess(queue);
            if (msg->mtype == DISCONNECT) {
                send_named_mess(server_queue, txt, DISCONNECT);
                end = 1;
                break;
            }
            
            printf("%s ", msg->mtext);

        }
        if (!end) {

            if (enter == 0) {
                printf("\n");
                enter = 1;
            }

            scanf("%s", txt);

            
            if (strcmp(txt, "\\end") == 0) {
                send_mess(chat_id, txt, DISCONNECT);
                send_named_mess(server_queue, txt, DISCONNECT);
                end = 1;
            }
            else if (strcmp(txt, "\\?"))
                send_mess(chat_id, txt, CONNECT);

        }
        

    } while (!end);
    mq_close(chat_id);

}

int main(int argc, char ** argv) {

    srand(time(NULL));
    signal(SIGINT, sigint);

    queue_name = calloc(NAME_SIZE, sizeof(char));
    strcpy(queue_name, "/client");  //7 -> 16 -1 >> 15 - 7 = 8
    for (int i = 7; i < NAME_SIZE - 1; ++i)
        queue_name[i] =  (char)('A' + rand() % 10);
    

   
    struct mq_attr queue_info;
    queue_info.mq_maxmsg = MAXMSG;
    queue_info.mq_msgsize = MESSAGE_LEN;
    queue = mq_open(queue_name, O_RDWR | O_CREAT, 0666, &queue_info);
    if (show_err("Queue make error")) return -1;
    
  
  
    server_queue = mq_open(argv[1], O_RDWR);
    if (show_err("Error opening server queue")) return -1;

    //char queue_key[100];
    //sprintf(queue_key, "%d", client_key);

    send_mess(server_queue, queue_name, INIT);
    if (show_err("Error sending message to server queue")) return -1;
    int ready = 0;
    char additional[20];
    char task[20];

    while (1) {

        if (id != -1) {
            ready = 1;
        }
        if (ready) {
        
            printf("[client %d]$ ", id);
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
                send_named_mess(server_queue, task, STOP);
                mq_close(queue);
                mq_unlink(queue_name);
                exit(0);
            }
            
        }
        sleep(1);
        
        while (has_messages(queue) > 0) {

            msgbuf * response;
            response = get_mess(queue);

            if (response->mtype == DISCONNECT) { 
                printf("server is down\n");
                sigint(0);
            }
                        
             if (!ready) 
                  id = atoi(response->mtext);             
              
             else {
                 if (response->mtype == CONNECT) {
                    char chat_name[NAME_SIZE];
                    strcpy(chat_name, response->mtext);
                    chat_id = mq_open(chat_name, O_RDWR);
                    chat();
                }
                else
                   printf("\n%s\n", response->mtext);
                    
                }
        }
        
    }
    
    mq_close(queue);
    mq_close(server_queue);
    mq_unlink(queue_name);

}
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
#include <mqueue.h>
#include "definitions.h"

mqd_t server_queue;
mqd_t users_qdesc[MAX_CLIENTS];
char * users_qnames[MAX_CLIENTS];
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

void send_mess(mqd_t queue, char * message, int mess_type) {
    
    char mess[MESSAGE_LEN - 10];
    sprintf(mess, "%c%s", (char)mess_type, message);

    mq_send(queue, mess, sizeof(mess), 1);
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

void sigint(int s) {
    char task[10];
    strcpy(task, "task");
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (users_qdesc[i] != 0) {
            send_mess(users_qdesc[i], task, DISCONNECT);
            mq_close(users_qdesc[i]);
        }
    }
    mq_close(server_queue);
    mq_unlink(server_queue_name);
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


int has_messages(mqd_t queue) {
    struct mq_attr queue_info;
    mq_getattr(queue, &queue_info);
    //printf("%ld %ld %ld %ld\n", queue_info.mq_curmsgs, queue_info.mq_flags, queue_info.mq_maxmsg, queue_info.mq_msgsize);
    return queue_info.mq_curmsgs;
}

int main(int argc, char ** argv) {

    signal(SIGINT, sigint);

    
    for (int i = 0; i < MAX_CLIENTS; ++i)  {
        users_qdesc[i] = 0; 
        users_qnames[i] = NULL;
        talks[i] = 0; 
    }


    char * buff;
    char num[100];
   

    printf("Server Queue Name: %s\n", server_queue_name);
    
    struct mq_attr qinfo;
    qinfo.mq_maxmsg = MAXMSG;
    qinfo.mq_msgsize = MESSAGE_LEN;

    server_queue = mq_open(server_queue_name, O_RDWR | O_CREAT, 0666, &qinfo);
    if (show_err("Queue make error")) return -1;


    msgbuf * response; 
        
  
    int client_id = -1;
    int chat_id = -1;
    while (1) {

        
        if (has_messages(server_queue))  {
            response = get_mess(server_queue);
            if (show_err("message receive error")) sigint(0);
        //else {
            
            printf("%s\n", response->mtext);
            switch (response->mtype) {
                case INIT:
                    printf("init\n");
                    for (int i = 0; i < MAX_CLIENTS; ++i) {
                        if (users_qdesc[i] == 0) {
                            users_qdesc[i] = mq_open(response->mtext, O_RDWR);
                            users_qnames[i] = calloc(NAME_SIZE, sizeof(char));
                            strcpy(users_qnames[i], response->mtext);
                            if (show_err("users i kolejak")) return -1;
                            char mess[100];
                            strcpy(mess, string(i));

                            send_mess(users_qdesc[i], mess, 1);
                            if (show_err("przesyl err")) return -1;
                            break;
                        }
                    }

                    break;

                case LIST:
                    get_named_mess(response->mtext, &client_id);
                    buff = calloc(1024, sizeof(char));
                    for (int i = 0; i < MAX_CLIENTS; ++i) {
                        if (users_qdesc[i] != 0) {
                            if (i != client_id && talks[i] != 1)
                                sprintf(num, "%d: 1 ", i);
                            else sprintf(num, "%d: 0 ", i);
                            strcat(buff, num);
                        }
                    }
                    send_mess(users_qdesc[client_id], buff, 1);
                    break;

                case CONNECT:
               
                    get_named_mess(response->mtext, &client_id);
                    chat_id = atoi(response->mtext);

                    if (client_id == chat_id) {
                       
                    }
                    else if (users_qdesc[chat_id] != 0 && !talks[chat_id]) {
                        printf("convo started\n");
                        sprintf(num, "%s", users_qnames[chat_id]);
                        send_mess(users_qdesc[client_id], num, CONNECT);
                        sprintf(num, "%s", users_qnames[client_id]);
                        send_mess(users_qdesc[chat_id], num, CONNECT);
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
                    mq_close(users_qdesc[client_id]);
                    users_qnames[client_id] = NULL;
                    users_qdesc[client_id] = 0;
                    talks[client_id] = 0;
                    break;

                default:
                    printf("err");
                    break;
            }
        //}
        }
    }

    

}
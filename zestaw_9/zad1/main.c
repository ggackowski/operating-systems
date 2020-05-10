#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <math.h>
#include <time.h>

#define new(Type) calloc(1, sizeof(Type))
#define New(Type, n) calloc(n, sizeof(Type))


void err(char * msg) {
    printf("!err: %s with errno of (%d: %s)\n", msg, errno, strerror(errno));
    exit(-1);
}

void create_and_run_thread(void * (*f) (void *), void * args) {
    pthread_t * thread = new (pthread_t);
    pthread_create(thread, NULL, f, args);
}


int chair_num, client_num;
int * waiting_room;
int occupied = -1;
int no_clients = 1;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void * barber(void * args) {

    while (1) {

        pthread_mutex_lock(&mutex);
        
        int cl_cnt = 0;
        int place = 0;
        for (int i = 0; i < chair_num; ++i) {
            if (waiting_room[i] != 0) {   
                occupied = waiting_room[i];
                place = i;
                cl_cnt++;
            }
        }


        if (cl_cnt == 0) no_clients = 1;
        else waiting_room[place] = 0;

        while (no_clients) {
            printf("Golibroda: idzie spac\n");
            pthread_cond_wait(&cond, &mutex);
        }
        
        pthread_mutex_unlock(&mutex);

        printf("Golibroda: czeka %d klientow, gole klienta %d\n", cl_cnt, occupied);
        
        sleep(rand() % 5);

    }

    return NULL;
}


void * client(void * args) {
    int id = *((int *) args);

    int free_cnt = 0;
    while (!free_cnt) {
    pthread_mutex_lock(&mutex);
    if (no_clients) {
        
        no_clients = 0;
        occupied = id;
        pthread_cond_broadcast(&cond);
        printf("Budze golibrode; %d\n", id);
    }
    else {
        free_cnt = 0;
        int place = -1;
        for (int i = 0; i < chair_num; ++i) {
            if (waiting_room[i] == 0) {
                free_cnt++;
                if (place == -1) {
                    place = i;
                    waiting_room[place] = id;
                }
            }
        }
        if (free_cnt != 0)
            printf("Poczekalnia, wolne miejsca: %d; %d\n", free_cnt, id);
        else printf("Zajete, %d\n", id);
     }
    pthread_mutex_unlock(&mutex);
    sleep(rand() % 5);
    }
    return NULL;
}

int main(int argc, char ** argv) {
    srand(time(NULL));
    if (argc != 3) err("Wrong amount of arguments");
    chair_num = atoi(argv[1]);
    client_num = atoi(argv[2]);

    waiting_room = New (int, chair_num);

    int * ids = New (int, client_num);
    for (int i = 0; i < client_num; ++i) ids[i] = i;

    create_and_run_thread(barber, NULL);

    for (int i = 0; i < client_num; ++i) {
        sleep(rand() % 5);
        create_and_run_thread(client, &ids[i]);
      
    }

    while(1) {}

    return 0;
}
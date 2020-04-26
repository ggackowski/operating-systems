#include "stuff.h"
#include "definitions.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

shared_memory * arr;
semaphores * sems;


void sigint(int s) {
    close_shared_memory(arr);
    exit(0);
}

int count_make(shared_memory * a) {
    int cnt = 0;
    for (int i = 1; i < a->size; ++i) 
        if (a->at[i].status == MAKE)    cnt++;
    return cnt;
}

int count_pack(shared_memory * a) {
    int cnt = 0;
    for (int i = 1; i < a->size; ++i) 
        if (a->at[i].status == PACK)    cnt++;
    return cnt;
}
void wait(int sem_id) {
    struct sembuf array bufs = new (struct sembuf, 1);

    bufs->sem_num = sem_id;
    bufs->sem_op = 0;
    
    semop(sems->id, bufs, 1);
}



int main(int argc, string array argv) {
    signal(SIGINT, sigint);
    srand(time(NULL));
    //printf("no\n");
    //printf("%s %s %s\n", argv[0], argv[1], argv[2]);
    arr = open_shared_memory(atoi(argv[1]), MAX_ORDERS + 1);
    sems = open_semaphore_set(atoi(argv[2]), MAKERS + PACKERS + SENDERS);
    int semaf_id = atoi(argv[3]);
    printf("sem id: %d\n", semaf_id);
    //printf("id: %d\n", arr->id); 2885812226 1157758978 

    while (1) {
        //printf("Czekam na semafor nr %d\n", semaf_id);
        wait(semaf_id);
        pid_t pid = getpid();
        int n = rand() % 10;
        int m = count_make(arr);
        int x = count_pack(arr);
        
        printf("(%d time) Dodalem liczbe: %d. Liczba zamownien do przygotowania: %d. Liczba zamownien do wyslania: %d\n", pid, n, m, x);
        

        arr->at[arr->at[0].size].status = MAKE;
        arr->at[arr->at[0].size].size = n;
        arr->at[0].size++;
        arr->at[0].size %= arr->size; 
        if (arr->at[0].size == 0)
            arr->at[0].size++;


        semaphore_increase(sems, semaf_id);
        printf("increased\n");
        //sleep(1);

    }
}
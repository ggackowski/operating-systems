#include "stuff.h"
#include "definitions.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <time.h>


long int now() {
    struct timespec ts;
    clock_gettime (CLOCK_MONOTONIC, &ts);
    return  (ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
}

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
    //semaphore_decrease(sems, sem_id);
}



int main(int argc, string array argv) {
    signal(SIGINT, sigint);
    srand(time(NULL));
    printf("no\n");
    printf("%s %s %s\n", argv[0], argv[1], argv[2]);
    arr = open_shared_memory(argv[1], MAX_ORDERS + 1);
    sems = open_semaphore_set(&argv[2], 1);
    //printf("sem id: %d\n", semaf_id);
    //printf("id: %d\n", arr->id); 2885812226 1157758978 

    while (1) {
        //printf("Czekam na semafor nr %d\n", semaf_id);
        sem_wait(sems->ids[0]);
        pid_t pid = getpid();
        int n = rand() % 10;
        int m = count_make(arr);
        int x = count_pack(arr);
        printf("(%d %ld) Dodalem liczbe: %d. Liczba zamownien do przygotowania: %d. Liczba zamownien do wyslania: %d\n", pid, now(), n, m, x);
        

        arr->at[arr->at[0].size].status = MAKE;
        arr->at[arr->at[0].size].size = n;
        arr->at[0].size++;
        arr->at[0].size %= arr->size; 
        if (arr->at[0].size == 0)
            arr->at[0].size++;


        //semaphore_decrease(sems, semaf_id);
        //printf("increased\n");
        //sleep(1);

    }
}
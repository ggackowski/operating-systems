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
    srand(time(NULL));
    signal(SIGINT, sigint);

    arr = open_shared_memory(atoi(argv[1]), MAX_ORDERS + 1);
    sems = open_semaphore_set(atoi(argv[2]), MAX_ORDERS);
    //printf("id: %d\n", arr->id);
    int semaf_id = atoi(argv[3]);
    printf("sem id: %d\n", semaf_id);

    while (1) {
        printf("Czekam na semafor nr %d\n", semaf_id);
        wait(semaf_id);
        int n;
        int found = 0;
        for (int i = 0; i < arr->size; ++i) {
            if (arr->at[i].status == PACK) {
                found = 1;
                n = arr->at[i].size * 3;
                arr->at[i].size = arr->at[i].status = 0;
                break;
            }
        }
        
        if (found) {
            pid_t pid = getpid();
            int m = count_make(arr);
            int x = count_pack(arr);
        
            printf("(%d time) Wyslalem zamowienie o wielkosci: %d. Liczba zamownien do przygotowania: %d. Liczba zamownien do wyslania: %d\n", pid, n, m, x);
        }

        sleep(1);
        semaphore_increase(sems, semaf_id);
        printf("increased\n");

    }
}
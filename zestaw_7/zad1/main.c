#include "stuff.h"
#include "definitions.h"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#define debug 1

shared_memory * memory;
semaphores * sems;
int workers = PACKERS + MAKERS + SENDERS;

void sigint(int s) {
    kill(0, SIGINT);

    delete_semaphore_set(sems);
    close_shared_memory(memory);
    delete_shared_memory(memory);
    exit(0);
}

void set_semaphores() {
    struct sembuf array bufs = new (struct sembuf, workers);
    for (int i = 0; i < workers; ++i) {
        bufs[i].sem_num = i;
        bufs[i].sem_op = 1;
    }
    semop(sems->id, bufs, workers);
    
    
}

int in(int array cont, int size, int v) {
    for (int i = 0; i < size; ++i) {
        if (cont[i] == v) return 1;
    }
    return 0;
}

int main() {

    signal(SIGINT, sigint);

    srand(time(NULL));
    memory = create_shared_memory(MAX_ORDERS + 1);
    //printf("SSSSS %d\n\n", memory->size);
    sems = create_semaphore_set(workers);

    set_semaphores();
    

    memory->at[0].size = 1;
    memory->at[0].status = -1;
    
    printf("key to mem: %d  key to sems: %d\n", memory->key, sems->key);
    
    string array arguments = new (string, 5);
    for (int i = 0; i < 5; ++i) {
        arguments[i] = new (char, 32);
    }
    int j = 0;
    sprintf(arguments[1], "%d", memory->key);
    sprintf(arguments[2], "%d", sems->key);
    arguments[4] = NULL;
    //printf("%s %s %s\n", arguments[0], arguments[1], arguments[2]);
    int makers[MAKERS];
    for (int i = 0; i < MAKERS; ++i) {
        makers[i] = j;
        sprintf(arguments[3], "%d", j++);
        //printf("ARG %s\n", arguments[3]);
        if (fork() == 0) {
            //printf("lol\n");
            execv("maker", arguments);
            //printf("no\n");
        }
    }

    for (int i = 0; i < PACKERS; ++i) {
        sprintf(arguments[3], "%d", j++);
        //printf("ARG %s\n", arguments[3]);
        if (fork() == 0) {
            execv("packer", arguments);
        }
    }


    for (int i = 0; i < SENDERS; ++i) {
        sprintf(arguments[3], "%d", j++);
        //printf("ARG %s\n", arguments[3]);
        if (fork() == 0) {
            execv("sender", arguments);
        }
    }
    int worker_id = 0;

    while (1) {
        //printf("%d\n", getpid());
        

        if (memory->at[memory->at[0].size].status == 0) {
            worker_id++;
            worker_id %= workers;
            semaphore_decrease(sems, worker_id);
        }
        else {
            do {
                worker_id++;
                worker_id %= workers;
            } while (in(makers, MAKERS, worker_id));
            semaphore_decrease(sems, worker_id);
        }
        if (debug) {
        printf("{ ");
        for (int i = 0; i < memory->size; ++i) {
            printf("[%d (%d)] ", memory->at[i].size, memory->at[i].status);
        }
        printf("}\n");
        for (int i = 0; i < sems->size; ++i) {
            printf("[%d] ", get_semaphore_value(sems, i));
        }
        printf("\n");
        printf("\n");
        }
        sleep(1);

    }
    
}

//1728184322
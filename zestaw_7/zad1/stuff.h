#ifndef _STUFF_H_
#define _STUFF_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>



#define array *
#define length sizeof 


#define new(Type, Size) calloc(Size,sizeof(Type))

//void array new(void array, int);

typedef char * string;

typedef struct {
    int size;
    int status;
} order;

typedef struct {
    int id;
    order array at;
    key_t key;
    size_t size;
} shared_memory;

typedef struct {
    int id;
    int size;
    key_t key;
} semaphores;

union semun {
               int              val;    /* Value for SETVAL */
               struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
               unsigned short  *Array;  /* Array for GETALL, SETALL */
               struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
    };



shared_memory * create_shared_memory(size_t size); 
shared_memory * open_shared_memory(key_t key, size_t size);
int close_shared_memory(shared_memory * sh_mem);
int delete_shared_memory(shared_memory * sh_mem);

semaphores * create_semaphore_set(int nsems);
semaphores * open_semaphore_set(key_t key, int size);
void semaphore_execute(semaphores * sems, int op, int index, short flag);
void semaphore_increase(semaphores * sems, int index);
void semaphore_decrease(semaphores * sems, int index);
void wait_for_semaphore(semaphores * sems, int index);
int get_semaphore_value(semaphores * sems, int index);
void delete_semaphore_set(semaphores * sems);



string string_copy(string str);

#endif
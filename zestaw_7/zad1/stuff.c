#include "stuff.h"
#include <errno.h>

string string_copy(string str) {
    string dest = new (dest, length(str));
    strcpy(dest, str);
    return dest;
}

shared_memory ptr create_shared_memory(size_t size) {
    key_t key = ftok(getenv("HOME"), rand() % 256);
    int id = shmget(key, size * sizeof(order), IPC_CREAT | 0666);
    order array arr = shmat(id, NULL, 0);
    shared_memory ptr shr_mem = new (shared_memory, 1);
    shr_mem->key = key;
    shr_mem->at = arr;
    shr_mem->id = id;
    shr_mem->size = size;
    return shr_mem;
}

shared_memory ptr open_shared_memory(key_t key, size_t size) {
    int id = shmget(key, 0, 0666);
    order array arr = shmat(id, NULL, 0);
    shared_memory ptr shr_mem = new (shared_memory, 1);
    shr_mem->key = key;
    shr_mem->at = arr;
    shr_mem->id = id;
    shr_mem->size = size;
    return shr_mem;
}

int close_shared_memory(shared_memory ptr sh_mem) {
    return shmdt(sh_mem->at);
}

void delete_shared_memory(shared_memory ptr sh_mem) {
    shmctl(sh_mem->id, IPC_RMID, NULL);
}

semaphores ptr create_semaphore_set(int nsems) {
    key_t key = ftok(getenv("HOME"), rand() % 256);
    int id = semget(key, nsems, IPC_CREAT | 0666);
    
    union semun arg;
    
    arg.val = 0;

    for (int i = 0; i < nsems; ++i) {
        semctl(id, i, SETVAL, arg);
    }

    semaphores ptr sems = new (semaphores, 1);
    sems->id = id;
    sems->key = key;
    sems->size = nsems;

    return sems;
}

semaphores ptr open_semaphore_set(key_t key, int size) {
    int id = semget(key, 0, 0666);
    printf("id: %d\n", id);
    printf("%s\n", strerror(errno));
    semaphores ptr sems = new (semaphores, 1);
    sems->id = id;
    sems->key = key;
    sems->size = size;
    
    return sems;

}

void semaphore_execute(semaphores ptr sems, int index, int op, short flag) {
    struct sembuf array s = new (struct sembuf, 1);
    s->sem_num = index;
    s->sem_op = op;
    s->sem_flg = flag;
    semop(sems->id, s, 1);
}

void semaphore_increase(semaphores ptr sems, int index) {
    semaphore_execute(sems, index, 1, 0);
}

void semaphore_decrease(semaphores ptr sems, int index) {
    semaphore_execute(sems, index, -1, 0);
}

void wait_for_semaphore(semaphores ptr sems, int index) {
    semaphore_execute(sems, index, 0, 0);
}

int get_semaphore_value(semaphores ptr sems, int index) {
    return semctl(sems->id, index, GETVAL);
}

void delete_semaphore_set(semaphores ptr sems) {
    semctl(sems->id, 0, IPC_RMID);
}

//void array new(void array type, int size) {
//    return calloc(size, sizeof(type));
//}


#include "stuff.h"
#include <errno.h>

string string_copy(string str) {
    string dest = new (dest, length(str));
    strcpy(dest, str);
    return dest;
}

shared_memory * create_shared_memory(size_t size) {
    string new_name = new (char, 32);
    new_name[0] = '\\';
    for (int i = 1; i < 31; ++i) new_name[i] = rand() % 14 + 'A';
    //printf("name %s\n", new_name);



    
    int id = shm_open(new_name, O_RDWR | O_CREAT, 0); 
    printf("id %d\n", id);
    printf("trunc: %d\n", ftruncate(id, size * sizeof(order)));
    order array addr = mmap(NULL, size * sizeof(order), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, id, 0);

    shared_memory * shr_mem = new (shared_memory, 1);
    shr_mem->name = new_name;
    shr_mem->at = addr;
    shr_mem->id = id;
    shr_mem->size = size;
    return shr_mem;
}

shared_memory * open_shared_memory(string name, size_t size) {
    int id = shm_open(name,  O_RDWR, 0);

    printf("id otw: %d\n", id);
    order array addr = mmap(NULL, size * sizeof(order), PROT_READ | PROT_EXEC | PROT_WRITE, MAP_SHARED, id, 0);

    shared_memory * shr_mem = new (shared_memory, 1);
    shr_mem->name = name;
    shr_mem->at = addr;
    shr_mem->id = id;
    shr_mem->size = size;
    return shr_mem;
}

int close_shared_memory(shared_memory * sh_mem) {
    return munmap(sh_mem->at, sh_mem->size);
}

int delete_shared_memory(shared_memory * sh_mem) {
     return shm_unlink(sh_mem->name);
}

semaphores * create_semaphore_set(int nsems) {
    semaphores * sems = new (semaphores, 1);
    sems->ids = new (sem_t *, nsems);
    sems->names = new (string, nsems);

    for (int i = 0; i < nsems; ++i) {
        string new_name = new (char, 32);
        new_name[0] = '\\';
        for (int i = 1; i < 31; ++i) new_name[i] = rand() % 14 + 'A';

        sem_t * id = sem_open(new_name, O_RDWR | O_CREAT, 0, 0);
        sems->ids[i] = id;
        sems->names[i] = new_name;
        sems->size = nsems;
    }    
    
    return sems;
}

semaphores * open_semaphore_set(string array names, int size) {
    semaphores * sems = new (semaphores, 1);
    sems->ids = new (sem_t *, size);
    sems->names = new (string, size);

    for (int i = 0; i < size; ++i) {
        sem_t * id = sem_open(names[i], O_RDWR);
        sems->ids[i] = id;
        sems->names[i] = names[i];
        sems->size = size;
    }
    
    printf("%s\n", strerror(errno));
    
    return sems;

}

void semaphore_execute(semaphores * sems, int index, int op, short flag) {
}

void semaphore_increase(semaphores * sems, int index) {
    sem_post(sems->ids[index]);
}

void semaphore_decrease(semaphores * sems, int index) {
    sem_wait(sems->ids[index]);
}

void wait_for_semaphore(semaphores * sems, int index) {
}

int get_semaphore_value(semaphores * sems, int index) {
    int val;
    sem_getvalue(sems->ids[index], &val);
    return val;
}

void close_semaphore_set(semaphores * sems) {
    for (int i = 0; i < sems->size; ++i) 
        sem_close(sems->ids[i]);
}

void delete_semaphore_set(semaphores * sems) {
    for (int i = 0; i < sems->size; ++i)
        sem_unlink(sems->names[i]);
}

//void array new(void array type, int size) {
//    return calloc(size, sizeof(type));
//}


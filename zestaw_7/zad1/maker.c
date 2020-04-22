#include "stuff.h"
#include "definitions.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>

int count_make(shared_memory ptr a) {
    int cnt = 0;
    for (int i = 1; i < a->size; ++i) 
        if (a->at[i].status == MAKE)    cnt++;
    return cnt;
}

int count_pack(shared_memory ptr a) {
    int cnt = 0;
    for (int i = 1; i < a->size; ++i) 
        if (a->at[i].status == PACK)    cnt++;
    return cnt;
}

int main(int argc, string array argv) {
    srand(time(NULL));
    shared_memory ptr arr = open_shared_memory(atoi(argv[1]), MAX_ORDERS + 1);
    printf("id: %d\n", arr->id);

    while (1) {
        pid_t pid = getpid();
        int n = rand() % 10;
        int m = count_make(arr);
        int x = count_pack(arr);
        
        printf("(%d time) Dodalem liczbe: %d. Liczba zamownien do przygotowania: %d. Liczba zamownien do wyslania: %d\n", pid, n, m, x);
        

        arr->at[arr->at[0].size].status = MAKE;
        arr->at[arr->at[0].size].size = n;
        arr->at[0].size++;
        arr->at[0].size %= arr->size;
        arr->at[0].size++;

        sleep(1);
    }
}
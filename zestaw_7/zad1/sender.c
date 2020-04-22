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
        int n;

        for (int i = 0; i < arr->size; ++i) {
            if (arr->at[i].status == PACK) {
                n = arr->at[i].size * 3;
                arr->at[i].size = arr->at[i].status = 0;
                break;
            }
        }

        pid_t pid = getpid();
        int m = count_make(arr);
        int x = count_pack(arr);
        
        printf("(%d time) Wyslalem zamowienie o wielkosci: %d. Liczba zamownien do przygotowania: %d. Liczba zamownien do wyslania: %d\n", pid, n, m, x);

        sleep(1);
    }
}
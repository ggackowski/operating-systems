#include "stuff.h"
#include "definitions.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, string array argv) {
    while (1) {
        srand(time(NULL));
        pid_t pid = getpid();
        int n = 3;
        int x = 2;
        printf("(%ld time) Dodalem liczbe: %d. Liczba zamownien do przygotowania: %d. Liczba zamownien do wyslania: %d", pid, n, x);
        sleep(1);
    }
}
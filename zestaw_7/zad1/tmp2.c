#include "stuff.h"
#include <stdlib.h>
#include <stdio.h>


int main(int argc, string array argv) {
    semaphores ptr sem = open_semaphore_set(atoi(argv[1]), atoi(argv[2]));
    semaphore_decrease(sem, 0);
    printf("decreased");
    printf("%d\n", get_semaphore_value(sem, 0));

}
#include "stuff.h"
#include <stdlib.h>
#include <stdio.h>


int main() {
    srand(time(NULL));
    //int ptr a = new (a, 1);
    //printf("%d", *a);
    //shared_memory ptr arr = create_shared_memory(17);

    //shared_memory ptr tst = new (shared_memory, 1);
    //printf("%ld\n", tst->size);
    //printf("%d\n", arr->key);
    //for (int i = 0; i < arr->size; ++i) {
    //    arr->at[i] = i + 1;
    //}
    semaphores ptr sems = create_semaphore_set(1);
    printf("%d\n", sems->key);
    printf("%d\n", get_semaphore_value(sems, 0));
    semaphore_increase(sems, 0);
    printf("%d\n", get_semaphore_value(sems, 0));
    semaphore_execute(sems, 0, 0, 0);

    printf("udalo sie\n");

    //delete_semaphore_set(sems);

    //close_shared_memory(arr);

}
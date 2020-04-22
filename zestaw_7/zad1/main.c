#include "stuff.h"
#include "definitions.h"

#include <stdio.h>

int main() {
    srand(time(NULL));
    shared_memory ptr memory = create_shared_memory(MAX_ORDERS + 1);
    memory->at[0].size = 1;
    memory->at[0].status = -1;
    printf("key: %d\n", memory->key);
}

//1728184322
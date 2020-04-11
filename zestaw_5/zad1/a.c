#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char ** argv) {
    //printf("1");
    printf("%d", atoi(argv[0]) + atoi(argv[1]));
    fflush(stdout);
}
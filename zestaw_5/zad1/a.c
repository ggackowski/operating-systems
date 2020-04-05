#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char ** argv) {
    fflush(stdout);
    printf("%d\n", atoi(argv[1]) + atoi(argv[2]));
    fflush(stdout);
}
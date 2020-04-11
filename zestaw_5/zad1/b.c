#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char ** argv) {
    char buff[100];
    scanf("%s", buff);
    printf("%d %d", atoi(argv[0]) + atoi(buff), atoi(argv[1]) + atoi(buff));
    fflush(stdout);
}
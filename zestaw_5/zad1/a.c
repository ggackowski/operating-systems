#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv) {
    //printf\n", argv[1], argv[2]);
    int i, j;
    //scanf("%d%d", &i, &j);
    printf("%d\n", atoi(argv[0]) + atoi(argv[1]));
    fflush(stdout);
}
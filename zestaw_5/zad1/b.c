#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv) {
    char buff[100];
    scanf("%s", buff);
    printf("!!!!!%s", buff);
    printf("%s_%s\n", argv[0], argv[1]);
    fflush(stdout);
}
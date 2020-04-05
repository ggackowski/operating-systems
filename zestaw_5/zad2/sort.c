#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char ** argv) {
    if (argc < 2) {
        printf("too little arguments");
        return 1;
    }
    char * command = calloc(strlen(argv[1]) + 5, sizeof(char));
    sprintf(command, "sort %s", argv[1]);
    FILE * f = popen(command, "r");
    while (!feof(f)) {
        size_t buff_size = 200;
        char * line = calloc(buff_size, sizeof(char));
        getline(&line, &buff_size, f);
        printf("%s", line);
    }
}
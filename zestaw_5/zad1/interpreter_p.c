
#define __USE_POSIX
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARGS 10
#define MAX_COMM 10

int main(int argc, char ** argv) {
    if (argc < 2) {
        printf("too little arguments\n");
        return -1;
    }

    FILE * f = fopen(argv[1], "r");
    char * line = NULL;
    char * line2 = NULL;
    size_t buff_size;
    size_t line_size = getline(&line, &buff_size, f);
    while (line_size != -1) {
        
        line2 = calloc(strlen(line) * 2, sizeof(char));
        int i = 0;
        printf("%s\n", line);
        char * tmp = strtok(line, "|");
        while (tmp) {
            printf("%s\n", tmp);
            strcat(line2, "./");
            strcat(line2, tmp);
            strcat(line2, " | ");
            ++i;
            strtok(NULL, "|");
        }
        printf("%s\n", line2);
        FILE * f = popen(line2, "r");

        int good = 0;
        while (!good) {
            good = 1;
            for (int j = 0; j < i; ++j) {
                if (!wait(NULL))
                    good = 0;
            }
        }
        printf("Ilosc potrzebnych procesow do wykonania: %d\n", i);
        sleep(1);
        line_size = getline(&line, &buff_size, f);

    }
}
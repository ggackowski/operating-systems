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
    size_t buff_size;
    size_t line_size = getline(&line, &buff_size, f);
    while (line_size != -1) {

        char * comm = strtok(line, "|");
        char ** comms = calloc(MAX_COMM, sizeof(char *));
        int i = 0;

        while (i < MAX_COMM && comm) {
            comms[i] = calloc(strlen(comm), sizeof(char));
            strcpy(comms[i], comm);
            comm = strtok(NULL, "|");
            ++i;
        }

        int k = 0;
        char buff[100];
        pid_t pids[MAX_COMM];
        while(comms[k]) {

            int i = 0;
            char * prog_name;
            char ** args = calloc(MAX_ARGS + 1, sizeof(char *));
            char * word = strtok(comms[k], " ");
            while (word) {
            //printf("word: %s\n", word);
                if (i == 0) {
                    prog_name = calloc(strlen(word), sizeof(char));
                    strcpy(prog_name, word);
                }
                else {
                    args[i] = calloc(strlen(word), sizeof(char)) ;   
                    strcpy(args[i], word);
                }
                ++i;
                word = strtok(NULL, " ");
            }
           // printf("Program name: %s\n", prog_name);
            //printf("args:");
          //  for (int i = 0; i < MAX_ARGS; ++i)
           //     printf(" %s", args[i]);
          //  printf("\n");
            int fd1[2];
            int fd2[2];
            pipe(fd1);
            pipe(fd2);
            pids[k] = fork();
            if (pids[k] == 0) {
                //child
                if (k > 0) {
                    close(fd2[1]);
                    dup2(fd2[0], STDIN_FILENO);
                }
                close(fd1[0]);
                dup2(fd1[1], STDOUT_FILENO);
                execv(prog_name, args);
                return 0;
            }
            else {
                if (k > 0) {
                    close(fd2[0]);
                    write(fd2[1], buff, 100);
                }
                close(fd1[1]);
                read(fd1[0], buff, 100);
                printf("%s\n", buff);
                
            }
            k++;
        }
        int good = 0;
        while (!good) {
            good = 1;
            for (int j = 0; j < k; ++j) {
                if (!wait(NULL))
                    good = 0;
            }
        }
        printf("Ilosc potrzebnych procesow do wykonania: %d\n", k);

        line_size = getline(&line, &buff_size, f);

    }
}
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <ftw.h>

char * arg;

int fn(const char * path, const struct stat * file_info, int flag, struct FTW * ftw) {
    if (S_ISDIR(file_info->st_mode)) {
        pid_t pid = fork();
        if (pid == 0) {
            printf("PID: %d, path: %s\n",pid, path); 
            char * command = calloc(sizeof(path) + 7, sizeof(char));
            strcat(command, "ls -l ");
            strcat(command, path);
            system(command);
        }
        else {
            wait(&pid);
        }
    }
    return 0;
}

void ls(char * path) {
  nftw(path, fn, 10, FTW_PHYS);
}

int main(int argc, char ** argv) {
    //printf("%s", argv[1]);
    arg = calloc(sizeof(argv[1]), sizeof(char));
    strcpy(arg, argv[1]);
    
    ls(arg);
}
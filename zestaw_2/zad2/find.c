#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>

int equals(char * str1, char * str2) {
  return (strcmp(str1, str2) == 0);
}

typedef struct dirent dirent;

void find_lib(char * path, int mtime, int atime, int maxdepth) {
    printf("mam sciezke %s i mtime %d atime %d i maxdepth %d", path, mtime, atime, maxdepth);
    DIR * dir = NULL;
    dir = opendir(path);
    dirent * file_in_dir = NULL;
    while ((file_in_dir = readdir(dir))) {
      printf("plik %s\n", file_in_dir.d_name);
    }
    closedir(dir);
}

int main(int argc, char ** argv) {
  int mtime = INT_MAX;
  int atime = INT_MAX;
  int maxdepth = INT_MAX;
  int i = 1;
  while (i < argc - 1) {
    if (equals(argv[i], "-mtime")) {
      if (i + 1 >= argc) {
        printf("ERR with argument to -mtime\n\n");
      }
      else if (atoi(argv[i + 1])) {
        mtime = atoi(argv[i + 1]);
      }
      else if (argv[i + 1][0] == '-' && atoi(argv[i + 1] + 1)) {
        mtime = (-1) * atoi(argv[i + 1] + 1);
      }
      else if (argv[i + 1][0] == '+' && atoi(argv[i + 1] + 1)) {
        mtime = atoi(argv[i + 1] + 1);
      } else {
        printf("ERR with argument to -mtime\n\n");
      }
      i++;
    }
    else if (equals(argv[i], "-atime")) {
      if (i + 1 >= argc) {
        printf("ERR with argument to -atime\n\n");
      }
      else if (atoi(argv[i + 1])) {
          atime = atoi(argv[i + 1]);
      }
      else if (argv[i + 1][0] == '-' && atoi(argv[i + 1] + 1)) {
          atime = (-1) * atoi(argv[i + 1] + 1);
      }
      else if (argv[i + 1][0] == '+' && atoi(argv[i + 1] + 1)) {
          atime = atoi(argv[i + 1] + 1);
      } else {
        printf("ERR with argument to -mtime\n\n");
      }
      i++;
    }
    else if (equals(argv[i], "-maxdepth")) {
      if (i + 1 >= argc) {
        printf("ERR with argument to -atime\n\n");
      }
      else if (atoi(argv[i + 1])) {
        maxdepth = atoi(argv[i + 1]);
      } else {
        printf("ERR with argument to -mtime\n\n");
      }
      i++;
    }
    i++;
  }
  if (i < argc)
    find_lib(argv[i], mtime, atime, maxdepth);
  else 
    printf("ERR no path\n\n");
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
int equals(char * str1, char * str2) {
  return (strcmp(str1, str2) == 0);
}

typedef struct dirent dirent;


void find_lib(char * path, int mtime, int atime, int maxdepth) {
  if (maxdepth == -1) return; 
    DIR * dir = NULL;
    struct stat * file_info = calloc(1, sizeof(struct stat));
    char * new_path = NULL;
    dir = opendir(path);
    dirent * file_in_dir = NULL;
    while ((file_in_dir = readdir(dir))) {
      printf("%s/%s\n", path, file_in_dir->d_name);
      new_path = calloc(sizeof(path) + sizeof(file_in_dir->d_name), sizeof(char));
      strcat(new_path, path);
      strcat(new_path, "/");
      strcat(new_path, file_in_dir->d_name);
      //printf("\n\n%s\n\n", new_path);
      lstat(new_path, file_info);
      //printf("%d\n", S_ISREG(file_info->st_mode));
      if (!equals(new_path + (strlen(new_path) - 2), "/.") && !equals(new_path  + (strlen(new_path) - 3), "/..") && S_ISDIR(file_info->st_mode)) {
        //printf("maszsiewypisa00");
        find_lib(new_path, mtime, atime, maxdepth - 1);
      }
      
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
        printf("ERR with argument to -atime\n\n");
      }
      i++;
    }
    else if (equals(argv[i], "-maxdepth")) {
      if (i + 1 >= argc) {
        printf("ERR with argument to maxdepth1\n\n");
      }
        
      
        maxdepth = atoi(argv[i + 1]);
     
      i++;
    }
    i++;
  }
  if (i < argc)
    find_lib(argv[i], mtime, atime, maxdepth);
  else 
    printf("ERR no path\n\n");
}
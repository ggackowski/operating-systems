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

  int mtime = INT_MIN + 1;
  int atime = INT_MIN + 1;
  int maxdepth = INT_MAX;
  int plus_a = 0;
  int plus_m = 0;

int equals(char * str1, char * str2) {
  return (strcmp(str1, str2) == 0);
}

typedef struct dirent dirent;


void find_lib(char * path, int mtime, int atime, int maxdepth, int plus_m, int plus_a) {
  time_t _time;
  time(&_time);
  if (maxdepth == 0) return; 
  DIR * dir = NULL;
  struct stat * file_info = calloc(1, sizeof(struct stat));
  char * new_path = NULL;
  dir = opendir(path);
  dirent * file_in_dir = NULL;
  while ((file_in_dir = readdir(dir))) {
      
      new_path = calloc(sizeof(path) + sizeof(file_in_dir->d_name), sizeof(char));
      strcat(new_path, path);
      strcat(new_path, "/");
      strcat(new_path, file_in_dir->d_name);
      lstat(new_path, file_info);
      long int m_time = (_time - file_info->st_mtime) / 86400;
      long int a_time = (_time - file_info->st_atime) / 86400;

      if ( ((plus_m == 1 && mtime >= 0 && m_time >= mtime) || (plus_m == 0 && mtime < 0 && m_time <= -mtime) || (plus_m == 0 && mtime >= 0 && m_time == mtime)) &&
           ((plus_a == 0 && atime < 0 && a_time <= -atime) || (plus_a == 0 && atime >= 0 && a_time == atime) || (plus_a == 1 && atime >= 0 && a_time >= atime)) ) {
        printf("%s/%s\t\t\t\t\t%ld\t", path, file_in_dir->d_name, file_info->st_nlink);
        if (S_ISDIR(file_info->st_mode))
         printf("dir");
      else if (S_ISCHR(file_info->st_mode)) 
        printf("char dev");
      else if (S_ISBLK(file_info->st_mode))
        printf("block dev");
      else if (S_ISFIFO(file_info->st_mode))
        printf("fifo");
      else if (S_ISLNK(file_info->st_mode)) 
        printf("slink");
      else if (S_ISREG(file_info->st_mode))
        printf("file");
      
      printf("\t%ld", file_info->st_size);
      printf("\t%ld\t%ld\n", file_info->st_atime, file_info->st_mtime);

      if (!equals(new_path + (strlen(new_path) - 2), "/.") && !equals(new_path  + (strlen(new_path) - 3), "/..") && S_ISDIR(file_info->st_mode)) {
        find_lib(new_path, mtime, atime, maxdepth - 1, plus_m, plus_a);
      }
      }
      
    }
  closedir(dir);
}

int fn(const char * path, const struct stat * file_info, int flag, struct FTW * ftw) {
  if (ftw->level > maxdepth) return 0;
  time_t _time;
  time(&_time);

      long int m_time = (_time - file_info->st_mtime) / 86400;
      long int a_time = (_time - file_info->st_atime) / 86400;

      if ( ((plus_m == 1 && mtime >= 0 && m_time >= mtime) || (plus_m == 0 && mtime < 0 && m_time <= -mtime) || (plus_m == 0 && mtime >= 0 && m_time == mtime)) &&
           ((plus_a == 0 && atime < 0 && a_time <= -atime) || (plus_a == 0 && atime >= 0 && a_time == atime) || (plus_a == 1 && atime >= 0 && a_time >= atime)) ) {
        printf("%s\t\t\t\t\t%ld\t", path, file_info->st_nlink);
        if (S_ISDIR(file_info->st_mode))
         printf("dir");
      else if (S_ISCHR(file_info->st_mode)) 
        printf("char dev");
      else if (S_ISBLK(file_info->st_mode))
        printf("block dev");
      else if (S_ISFIFO(file_info->st_mode))
        printf("fifo");
      else if (S_ISLNK(file_info->st_mode)) 
        printf("slink");
      else if (S_ISREG(file_info->st_mode))
        printf("file");
      else
        printf("file");
      printf("\t%ld", file_info->st_size);
      printf("\t%ld\t%ld\n", file_info->st_atime, file_info->st_mtime);
       }
    
    return 0;
}

void find_sys(char * path) {
  nftw(path, fn, 10, FTW_PHYS);
}

int main(int argc, char ** argv) {
  int i = 1;
  while (i < argc - 1) {
    if (equals(argv[i], "-mtime")) {
      if (i + 1 >= argc) {
        printf("ERR with argument to -mtime\n\n");
      }
      else if (argv[i + 1][0] == '-' && (atoi(argv[i + 1] + 1) || argv[i + 1][0] == '0' || argv[i + 1][1] == '0')) {
        mtime = (-1) * atoi(argv[i + 1] + 1);
        plus_m = 0;
      }
      else if (argv[i + 1][0] == '+' && (atoi(argv[i + 1] + 1) || argv[i + 1][0] == '0' || argv[i + 1][1] == '0')) {
        mtime = atoi(argv[i + 1] + 1);
        plus_m = 1;
      } 
      else if (atoi(argv[i + 1]) || argv[i + 1][0] == '0' || argv[i + 1][1] == '0') {
        mtime = atoi(argv[i + 1]);
      }
      else {
        printf("ERR with argument to -mtime\n\n");
      }
      i++;
    }
    else if (equals(argv[i], "-atime")) {
      if (i + 1 >= argc) {
        printf("ERR with argument to -atime\n\n");
      }
      else if (argv[i + 1][0] == '-' && (atoi(argv[i + 1] + 1) || argv[i + 1][0] == '0' || argv[i + 1][1] == '0')) {
          atime = (-1) * atoi(argv[i + 1] + 1);
          plus_a = 0;
      }
      else if (argv[i + 1][0] == '+' && (atoi(argv[i + 1] + 1) || argv[i + 1][0] == '0' || argv[i + 1][1] == '0')) {
          atime = atoi(argv[i + 1] + 1);
          plus_a = 1;
      } 
      else if (atoi(argv[i + 1]) || argv[i + 1][0] == '0' || argv[i + 1][1] == '0') {
          atime = atoi(argv[i + 1]);
      }
      else {
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
  if (i < argc) {
    find_sys(argv[i]);
  }
  else 
    printf("ERR no path\n\n");

}
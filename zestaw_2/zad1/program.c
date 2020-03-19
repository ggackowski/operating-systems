#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/times.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>  
#include <sys/stat.h>
#include <unistd.h>

double time_difference(clock_t start, clock_t stop) {
	double time_diff = (double)(stop - start) / sysconf(_SC_CLK_TCK);
	return time_diff;
}

int compare_strings(char *s1, char *s2) {
    int i;

    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0')
            return 0;
    }
    if ((unsigned char)s1[i] < (unsigned char)s2[i])
        return -1;
    else
        return 1;
}

char * generate_str(int len) {
    char * str = calloc(len, sizeof(char));
    int i;
    for (i = 0; i < len; ++i)
        str[i] = (char)(rand() % 25 + 65);
   ;
    //printf("%s\n", str);
    return str;

}

char * lib_read_block(FILE * f, int index, int len){
    char * block = calloc(len, sizeof(char));
    fseek(f, (len) * index, 0);
    fread(block, sizeof(char), len, f);
    return block;
}

void lib_save_to_block(FILE * f, char * block, int index, int len){
    fseek(f, (len) * index, 0);
    fwrite(block, sizeof(char), len, f);
}

void lib_generate(char * filename, int num_of_str, int str_len) {
    FILE * f = NULL;
    char * string = NULL;
    if ((f = fopen(filename, "w")) < 0) 
        printf("nie udalo sie otworzyc pliku %s\n\n", filename);
    int i;
    for (i = 0; i < num_of_str; ++i) {
        string = generate_str(str_len);
        fprintf(f, "%s", string);
    }
    fclose(f);   
}

void lib_copy(char * file1, char * file2,  int cnt_of_rec, int buffor_size, int rec_len) {
    FILE * f1 = NULL;
    FILE * f2 = NULL;
    if ((f1 = fopen(file1, "r")) < 0) 
        printf("nie udalo sie otworzyc pliku %s\n\n", file1);    
    if ((f2 = fopen(file2, "w")) < 0) 
        printf("nie udalo sie otworzyc pliku %s\n\n", file2);
    char * buffor = calloc(buffor_size, sizeof(char));
    int cnt = 0;
    int i = 0;
   // printf("%d, %d\n", cnt_of_rec * rec_len, i);
    while ((cnt = fread(buffor, sizeof(char), buffor_size, f1))) {
        fwrite(buffor, sizeof(char), cnt, f2);
       // printf("%s + %d\n", buffor, i);
        i += buffor_size;
    }
    fclose(f1);
    fclose(f2);
}

int lib_partition(FILE * file, int low, int high, int len)  
{  
    char * pivot = lib_read_block(file, high, len);  
    //printf("%s, \n", pivot);
    char * file_at_j;

    int i = (low - 1);  
    int j;
  
    for (j = low; j <= high - 1; j++)  
    {  
        file_at_j = lib_read_block(file, j, len);
        //printf("compare %s z %s\n", file_at_j, pivot);
        if (compare_strings(file_at_j, pivot) < 0)  
        {  
            //printf("%s mniejszy od piwot %s\n", file_at_j, pivot);
            free(pivot);
            i++;   
            //file_at_i = lib_read_block(file, i, len);
            pivot = lib_read_block(file, i, len);
            //printf("Zamiana %s z %s\n", pivot, file_at_j);
            lib_save_to_block(file, file_at_j, i, len);
            lib_save_to_block(file, pivot, j, len);
            //arr[i] = arr[j];
            //arr[j] = tmp;
            free(pivot);
            pivot = lib_read_block(file, high, len);  
        }  
    }  
    free(pivot);
    free(file_at_j);
    pivot = lib_read_block(file, i + 1, len);
    lib_save_to_block(file, lib_read_block(file, high, len), i + 1, len);
    lib_save_to_block(file, pivot, high, len);
    //tmp = arr[i + 1];
    //arr[i + 1] = arr[high];
    //arr[high] = tmp;
    free(pivot);
    //printf("%d\n", i + 1); 
    return (i + 1);  
}  
  
void lib_quick_sort(FILE * file, int low, int high, int len)  
{  
    if (low < high)  
    {  
        //(printf("od %d fo %d\n", low, high);
        int pivot = lib_partition(file, low, high, len);  
        lib_quick_sort(file, low, pivot - 1, len);  
        lib_quick_sort(file, pivot + 1, high, len);  
    }  
} 

char * sys_read_block(int fd, int index, int length){
    char * block = calloc(length, sizeof(char));
    lseek(fd, length * index, 0);
    read(fd, block, length);
    return block;
}

void sys_save_to_block(int fd, char * block, int index, int len){
    lseek(fd, len * index, 0);
    write(fd, block, len);
}

int sys_partition(int file, int low, int high, int len)  
{  
    char * pivot = sys_read_block(file, high, len);  
    //printf("%s, \n", pivot);
    char * file_at_j;

    int i = (low - 1);  
    int j;
  
    for (j = low; j <= high - 1; j++)  
    {  
        file_at_j = sys_read_block(file, j, len);
        if (compare_strings(file_at_j, pivot) < 0)  
        {  
            //printf("%s mniejszy od piwot %s\n", file_at_j, pivot);
            free(pivot);
            i++;   
            //file_at_i = lib_read_block(file, i, len);
            pivot = sys_read_block(file, i, len);
            //printf("Zamiana %s z %s\n", pivot, file_at_j);
            sys_save_to_block(file, file_at_j, i, len);
            sys_save_to_block(file, pivot, j, len);
            //arr[i] = arr[j];
            //arr[j] = tmp;
            free(pivot);
            pivot = sys_read_block(file, high, len);  
        }  
    }  
    free(pivot);
    free(file_at_j);
    pivot = sys_read_block(file, i + 1, len);
    sys_save_to_block(file, sys_read_block(file, high, len), i + 1, len);
    sys_save_to_block(file, pivot, high, len);
    //tmp = arr[i + 1];
    //arr[i + 1] = arr[high];
    //arr[high] = tmp;
    free(pivot);
    //printf("%d\n", i + 1); 
    return (i + 1);  
}  

void sys_quick_sort(int file, int low, int high, int len)  
{  
    if (low < high)  
    {  
        //printf("od %d fo %d\n", low, high);
        int pivot = sys_partition(file, low, high, len);  
        sys_quick_sort(file, low, pivot - 1, len);  
        sys_quick_sort(file, pivot + 1, high, len);  
    }  
} 

void sys_copy(char * f1, char * f2, int record_num, int buff_size){
    int file1, file2;
    file1 = open(f1, O_RDONLY);
    file2 = open(f2, O_WRONLY);
    
    char * buffer = calloc(buff_size, sizeof(char));
    int i;
    for (i = 0; i < record_num; ++i){
        read(file1, buffer, buff_size);
        write(file2, buffer, buff_size);
    }
    close(file1);
    close(file2);
}


int main(int argc, char ** argv) {
   // lib_generate("file_gen.txt", 10, 11);
    //lib_copy("file_gen.txt", "plik2", 5, 2, 11);

    srand(time(NULL));
    struct tms * tms_start = calloc(1, sizeof(struct tms));
	struct tms * tms_stop = calloc(1, sizeof(struct tms));
	clock_t clock_start = 0;
	clock_t clock_stop = times(tms_stop);

    int i = 1;
    int rec_len = 0;
    int err = 0;

    while (i < argc) {
        printf("Wykonanie: \"%s\"\n", argv[i]);
		clock_start = times(tms_start);
        if (strcmp(argv[i], "generate") == 0) {
            if (i + 3 < argc) {
              lib_generate(argv[(i + 1)], atoi(argv[i + 2]), atoi(argv[i + 3]));
              i += 3;  
              rec_len = atoi(argv[i + 3]);
            }
            else err = 1;
        }
        else if (strcmp(argv[i], "sort") == 0) {
            if (i + 4 < argc && strcmp(argv[i + 4], "sys") == 0) {
                int file = open(argv[i + 1], O_RDWR);
                sys_quick_sort(file, 0, atoi(argv[i + 2]) - 1, atoi(argv[i + 3]));
                 i += 4;
            }
            else if (i + 4 < argc && strcmp(argv[i + 4], "lib") == 0) {
                FILE * f = fopen(argv[i + 1], "r+");
                lib_quick_sort(f, 0, atoi(argv[i + 2]) - 1, atoi(argv[i + 3]));
                i += 4;
            }
            else err = 1;
        }
        else if (strcmp(argv[i], "copy") == 0) {
            if (i + 5 < argc && strcmp(argv[i + 5], "sys") == 0) {
              sys_copy(argv[(i + 1)], argv[i + 2], atoi(argv[i + 3]), atoi(argv[i + 4]));
              i += 5;  
            }
            else if (i + 5 < argc && strcmp(argv[i + 5], "lib") == 0) {
                lib_copy(argv[(i + 1)], argv[i + 2], atoi(argv[i + 3]), atoi(argv[i + 4]), rec_len);
                i += 5;
            }
            else err = 1;
        }
        if (err) {
            printf("nieprawidlowy parametr: %d.%s\n", i, argv[i]);
            break;
        }
        
        clock_stop = times(tms_stop);
		printf("real time: %f\n", time_difference(clock_start, clock_stop));
		printf("system time: %f\n", time_difference(tms_start->tms_stime, tms_stop->tms_stime));
		printf("user time: %f\n\n\n", time_difference(tms_start->tms_utime, tms_stop->tms_utime));
        
        i++;
    }
}
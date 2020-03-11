#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
//#include <sys/times.h>
#include <memory.h>
//#include <unistd.h>

/*
double time_difference(clock_t start, clock_t stop) {
	double time_diff = (double)(stop - start) / sysconf(_SC_CLK_TCK);
	return time_diff;
}*/

char * generate_str(int len) {
    char * str = calloc(len, sizeof(char));
    int i;
    for (i = 0; i < len - 1; ++i)
        str[i] = (char)(rand() % 25 + 65);
    str[len - 1] = '\0';
    //printf("%s\n", str);
    return str;

}

char * lib_read_block(FILE * f, int index, int len){
    char * block = calloc(len, sizeof(char));
    fseek(f, (len + 1) * index, 0);
    fread(block, sizeof(char), len, f);
    return block;
}

void lib_save_to_block(FILE * f, char * block, int index, int len){
    fseek(f, (len + 1) * index, 0);
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
    while ((i < (cnt_of_rec * rec_len)) && (cnt = fread(buffor, sizeof(char), buffor_size, f1))) {
        fwrite(buffor, sizeof(char), cnt, f2);
       // printf("%s + %d\n", buffor, i);
        i += buffor_size;
    }
    fclose(f1);
    fclose(f2);
}

void writeBlock_sys(int fd, char * block, int index, int len){
    lseek(fd, (len + 1) * index, 0);
    write(fd, block, len);
}

void copy_sys(char * f1, char * f2, int records, int len){
    int fd1, fd2;
    if ( (fd1 = open(f1, O_RDONLY)) < 0){
        error("copy_sys open1");
    }
    if ( (fd2 = open(f2, O_WRONLY)) < 0){ // O_CREAT
        error("copy_sys open2");
    }
    char buff[len + 1];
    int i, rd, wr;
    for (i = 0; i < records; ++i){
        if ( (rd = read(fd1, buff, len + 1)) != len + 1){
            error("copy_sys read");
        }
        if ( (wr = write(fd2, buff, len + 1)) != len + 1){
            error("copy_sys write");
        }
    }
    close(fd1);
    close(fd2);
}

int main(int argc, char ** argv) {
   // lib_generate("file_gen.txt", 10, 11);
    lib_copy("file_gen.txt", "plik2", 5, 2, 11);

/*
    struct tms * tms_start = calloc(1, sizeof(struct tms));
	struct tms * tms_stop = calloc(1, sizeof(struct tms));
	clock_t clock_start = 0;
	clock_t clock_stop = times(tms_stop);
*/
    int i = 1;
    int rec_len = 0;
    while (i < argc) {
        printf("Wykonanie: \"%s\"\n", argv[i]);
		//clock_start = times(tms_start);
        if (strcmp(argv[i], "generate") == 0) {
            if (i + 4 < argc && strcmp(argv[i + 4], "sys") == 0) {
              printf("soon bedzie gen\n");
              i += 4;  
            }
            else {
                lib_generate(argv[(i + 1)], atoi(argv[i + 2]), atoi(argv[i + 3]));
                rec_len = atoi(argv[i + 3]);
                i += 3;
            }
        }
        else if (strcmp(argv[i], "sort") == 0) {
            if (i + 4 < argc && strcmp(argv[i + 4], "sys") == 0) {
              printf("soon bedzie sort\n");
              i += 4;  
            }
            else {
                //lib_sort(argv[(i + 1)], atoi(argv[i + 2]), atoi(argv[i + 3]));
                i += 3;
            }
        }
        else if (strcmp(argv[i], "copy") == 0) {
            if (i + 5 < argc && strcmp(argv[i + 5], "sys") == 0) {
              printf("soon bedzie copy\n");
              i += 5;  
            }
            else {
                lib_copy(argv[(i + 1)], argv[i + 2], atoi(argv[i + 3]), atoi(argv[i + 4]), rec_len);
                i += 4;
            }
        }
        /*
        clock_stop = times(tms_stop);
		printf("real time: %f\n", time_difference(clock_start, clock_stop));
		printf("system time: %f\n", time_difference(tms_start->tms_stime, tms_stop->tms_stime));
		printf("user time: %f\n\n\n", time_difference(tms_start->tms_utime, tms_stop->tms_utime));
        
        */i++;
    }
}
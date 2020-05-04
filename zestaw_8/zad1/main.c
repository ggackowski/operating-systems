#define SIGN 1
#define BLOCK 2
#define INTERLEAVED 3

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <math.h>

#define new(Type) calloc(1, sizeof(Type))
#define New(Type, n) calloc(n, sizeof(Type))

int threads_cnt;
int mode;
char input_filename[128];
char output_filename[128];

int width_img;
int height_img;
int max_val;
int max;
short ** input_img;
short ** output_img;

int histogram[256];
int args[2];

pthread_t ** threads = NULL;
int thread_i = 0;

void create_and_run_thread(void * (*f) (void *), void * args) {
    if (threads == NULL) {
        threads = New (pthread_t, threads_cnt);
    }

    threads[thread_i] = new (pthread_t);
    pthread_create(threads[thread_i], NULL, f, args);
    thread_i++;
}

void error(char * err) {
    printf("[ERR] %s\n", err);
    exit(-1);
}

long gettime() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return 1000000 * tv.tv_sec + tv.tv_usec;
}

long * time_diff(long b, long e) {
    long * t = new (long);
    *t = e - b;
    return t;
}

void read_image() {
    FILE * input_file = fopen(input_filename, "r");
    char buff[32];
    fgets(buff, 32, input_file);
    fscanf(input_file, "%d%d%d", &width_img, &height_img, &max_val);

    input_img = New (short *, height_img);
    for (int i = 0; i < height_img; ++i) 
        input_img[i] = New (short, width_img);
    
    for (int i = 0; i < height_img; ++i) {
        for (int j = 0; j < width_img; ++j) {
            fscanf(input_file, "%hd", &input_img[i][j]);
        }
    }

    fclose(input_file);

}

void save_image() {
    FILE * output_file = fopen(output_filename, "w");
    fprintf(output_file, "P2\n%d %d\n%d\n", max + 1, max_val, 255);
    for (int i = 0; i < max_val; ++i) {
        for (int j = 0; j <= max; ++j)
            if (j <= histogram[i]) 
                fprintf(output_file, "%d ", 255);
            else 
                fprintf(output_file, " %d ", 0);
        fprintf(output_file, "\n");
    }
    fclose(output_file);
}

int ** histogram_parts;

void * count_pixels_in_set(void * args) {
    long begin  = gettime();
    int k = *((int *)args);
    int m1 = 256 / threads_cnt;
//    printf("%d %d\n", k, m1);

    for (int i = 0; i < height_img; ++i)
        for (int j = 0; j < width_img; ++j) 
            if (input_img[i][j] >= (k * m1) && input_img[i][j] < ((k + 1) * m1)) 
                histogram_parts[k][input_img[i][j]]++;

    long end = gettime();
    pthread_exit((void *) time_diff(begin, end));
    return NULL;
}

void * count_pixels_in_block(void * args) {
    long begin  = gettime();
    int k = *((int *)args);
    int m2 = (int)ceil((double)width_img / threads_cnt);
//    printf("watek z k = %d i m = %d\n", k, m2);

    for (int i = 0; i < height_img; ++i)
        for (int j = 0; j < width_img; ++j) 
            if(j >= (k) * m2 && j <= (k + 1) * m2 - 1) 
                histogram_parts[k][input_img[i][j]]++;
        
    long end = gettime();
    pthread_exit((void *) time_diff(begin, end));
    return NULL;
}

void * count_pixels_in_interval(void * args) {
    long begin  = gettime();
    int k = *((int *)args);
   // printf("proces z k %d\n", k);

    for (int i = 0; i < height_img; ++i)
        for (int j = k; j < width_img; j += threads_cnt) 
                histogram_parts[k][input_img[i][j]]++;

    long end = gettime();
    pthread_exit((void *) time_diff(begin, end));
    return NULL;
}

int main(int argc, char ** argv) {
    
    if (argc < 5) error("Too little arguments");
    threads_cnt = atoi(argv[1]);
    
    if (strcmp(argv[2], "sign") == 0)
        mode = SIGN;
    else if (strcmp(argv[2], "block") == 0)
        mode = BLOCK;
    else if (strcmp(argv[2], "interleaved") == 0)
        mode = INTERLEAVED;
    else error("Wrong argument 2");

    if (!strcpy(input_filename, argv[3])) error("problem with arg 3");
    if (!strcpy(output_filename, argv[4])) error("problem with arg 4");  

    histogram_parts = New (int *, threads_cnt);
    for (int i = 0; i < threads_cnt; ++i)
        histogram_parts[i] = New (int, 256);

    read_image();

    int * args = New (int, threads_cnt);

    for (int i = 0; i < threads_cnt; ++i) {
        args[i] = i;
    }
    long begin = gettime();

    if (mode == SIGN) 
        for (int i = 0; i < threads_cnt; ++i) 
            create_and_run_thread(count_pixels_in_set, ((void *) &args[i]));

    else if (mode == BLOCK) 
        for (int i = 0; i < threads_cnt; ++i) 
            create_and_run_thread(count_pixels_in_block, ((void *) &args[i]));
    
    else if (mode == INTERLEAVED) 
        for (int i = 0; i < threads_cnt; ++i) 
            create_and_run_thread(count_pixels_in_interval, ((void *) &args[i]));

    for (int i = 0; i < threads_cnt; ++i) {
        void * result;
        pthread_join(*threads[i], &result);
        printf("Czas wykonania watku %d: %ld\n", i, *((long *)result));
    }

    long end = gettime();

    printf("Calkowity czas: %ld\n", *time_diff(begin, end));

    for (int i = 0; i < threads_cnt; ++i) {
        for (int j = 0; j < 256; ++j) 
            histogram[j] += histogram_parts[i][j];
    }

    max = 0;
    for (int i = 0; i < 256; ++i) {
        if (histogram[i] > max) {
            max = histogram[i];
        }
    }

    save_image();

    return 0;
}
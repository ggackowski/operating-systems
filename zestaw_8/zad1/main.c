#define SIGN 1
#define BLOCK 2
#define INTERLEAVED 3

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define new(Type) calloc(1, sizeof(Type))
#define New(Type, n) calloc(n, sizeof(Type))

pthread_t * create_and_run_thread(void * (*f) (void *), void * args) {
    pthread_t * thread = new (pthread_t);
    pthread_create(thread, NULL, f, args);
    return thread;
}

void * print(void * a) {
    while (1) {
        printf("watek\n");
        sleep(1);
    }
    return NULL;
}

void error(char * err) {
    printf("[ERR] %s\n", err);
    exit(-1);
}

int threads_cnt;
int mode;
char input_filename[128];
char output_filename[128];

int width_img;
int height_img;
int max_val;
short ** input_img;
short ** output_img;

int histogram[256];
int args[2];

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

void create_output_file() {
    output_img = New (short *, max_val + 1);
    for (int i = 0; i <= max_val; ++i) 
        output_img[i] = New (short, width_img * height_img);
}

void save_image() {
    FILE * output_file = fopen(output_filename, "w");
    printf("%s", strerror(errno));
    fprintf(output_file, "P2\n%d %d\n%d\n", width_img * height_img, max_val + 1, 250);
    for (int i = 0; i <= max_val; ++i) {
        for (int j = 0; j < width_img * height_img; ++j)
            if (j <= histogram[i]) 
                fprintf(output_file, "%d ", 256);
            else 
                fprintf(output_file, " %d ", 0);
        fprintf(output_file, "\n");
    }
    fclose(output_file);
}

int in(int what, int * arr, int s) {
    for (int i = 0; i < s; ++i) 
        if (arr[i] == what) 
            return 1;
    return 0;
}

void * count_pixels_in_set() {
    int k = args[0];
    int m = args[1];
    args[0]++;
    printf("%d %d\n", k, m);

    for (int i = 0; i < height_img; ++i)
        for (int j = 0; j < width_img; ++j) {
            if (input_img[i][j] >= (k * m) && input_img[i][j] < ((k + 1) * m)) {
                histogram[input_img[i][j]]++;
                printf("ok");
            }
        }
    return NULL;
}

void * count_pixels_in_block() {
    int k = args[0];
    int m = args[1];
    args[0]++;
    printf("%d %d\n", k, m);

    for (int i = 0; i < height_img; ++i)
        for (int j = 0; j < width_img; ++j) {
            if(j >= (k - 1) * ceil(m) && j <= k * ceil(m) - 1) {
                histogram[input_img[i][j]]++;
                printf("ok");
            }
        }
    return NULL;
}

void * count_pixels_in_interval() {
    int k = args[0];

    args[0]++;

    printf("w %d\n", k);

    for (int i = 0; i < height_img; ++i)
        for (int j = 0; j < width_img; ++j) {
            if((j - k + 1) % threads_cnt == 0) {
                histogram[input_img[i][j]]++;
                printf("ok");
            }
        }
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

    read_image();

    create_output_file();

    for (int i = 0; i < 256; ++i) histogram[i] = 0;
    
    if (mode == SIGN) {
        int m = 256 / threads_cnt;
        args[0] = 0;
        args[1] = m;
        for (int i = 0; i < threads_cnt; ++i) {
            printf("%d", i);
            create_and_run_thread(count_pixels_in_set, NULL);
        }
    }
    else if (mode == BLOCK) {
        int m = width_img / threads_cnt;
        args[0] = 0;
        args[1] = m;
        for (int i = 0; i < threads_cnt; ++i) {
            printf("%d", i);
            create_and_run_thread(count_pixels_in_block, NULL);
        }
    }
    else if (mode == INTERLEAVED) {
        int m = width_img / threads_cnt;
        args[0] = 0;
        args[1] = 0;
        for (int i = 0; i < threads_cnt; ++i) {
            printf("%d", i);
            create_and_run_thread(count_pixels_in_interval, NULL);
        }
    }

    printf("C"); fflush(stdout);
    save_image();

    create_and_run_thread(print, NULL);

    char buff[32];

    while(1) {
        scanf("%s", buff);
        printf("%s %s\n", buff, buff);
    }
    
    return 0;
}
#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 200
#define offset 2 * sizeof(int)

void write_value(FILE * file, int cols, int rows, float value, unsigned int r, unsigned int c)
{
    fseek(file, sizeof(int) + (cols * r + c) * sizeof(float), 0);
    fwrite(&value, sizeof(float), 1, file);
}

float read_value(FILE * file, int rows, int cols, unsigned int r, unsigned int c)
{
    float value;
    fseek(file, sizeof(int) + (cols * r + c) * sizeof(float), 0);
    fscanf(file, "%f", &value);
    return value;
}

void show_matrix(FILE * file, int rows, int cols)
{
    float v;
    for (unsigned int r = 0; r < rows; r++)
    {
        for (unsigned int c = 0; c < cols; c++)
        {
            v = read_value(file,rows, cols, r, c);
            printf("%f ", v);
        }
        printf("\n");
    }
}

int main(int argc, char ** argv) {
    //arg 1 - plik lista
    //arg 2 - liczba procesow
    FILE * f = fopen(argv[1], "r+");
    int i;
    char * matrix1_filename = calloc(MAX_SIZE, sizeof(char));
    char * matrix2_filename = calloc(MAX_SIZE, sizeof(char));
    char * matrix_res_filename = calloc(MAX_SIZE, sizeof(char));
    fscanf(f, "%s", matrix1_filename);
    fscanf(f, "%s", matrix2_filename);
    fscanf(f, "%s", matrix_res_filename);
    FILE * a = fopen(matrix1_filename, "r+");
    show_matrix(a, 3, 3);
    write_value(a, 3, 3, 1.3, 1, 1);
    int num_of_processes = atoi(argv[2]);
    pid_t * PIDs = calloc(num_of_processes, sizeof(pid_t));

    for (i = 0; i < num_of_processes; ++i) {
       // PIDs[i] = fork();
        printf("%d\n", i);
    }
    printf("proces");
}
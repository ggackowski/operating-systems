#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char * generate_str(int len) {
    char * str = calloc(len, sizeof(char));
    int i;
    for (i = 0; i < len - 1; ++i)
        str[i] = rand() % 21 + 65;
    str[len - 1] = '\0';
    return str;

}

void lib_generate(char * filename, int num_of_str, int str_len) {
    
}

void lib_copy(char * file1, char * file2, int buffor_size) {
    FILE * f1 = NULL;
    FILE * f2 = NULL;
    if ((f1 = fopen(file1, "r")) < 0) 
        printf("nie udalo sie otworzyc pliku %s\n\n", file1);

    
    if ((f2 = fopen(file2, "w")) < 0) 
        printf("nie udalo sie otworzyc pliku %s\n\n", file2);

    char * buffor = calloc(buffor_size, sizeof(char));
    int cnt = 0;
    while ((cnt = fread(buffor, sizeof(char), buffor_size, file1))) {
        fwrite(buffor, sizeof(char), cnt, file2);
    }
    fclose(f1);
    fclose(f2);
}

int main() {
    FILE * f1 = fopen("plik1", "r");
    FILE * f2 = fopen("plik2", "w");
    copy_lib(f1, f2, 16);
}
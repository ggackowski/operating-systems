#include <stdio.h>
#include <stdlib.h>

void copy_lib(FILE * file1, FILE * file2, int buffor_size) {
    char * buffor = calloc(buffor_size, sizeof(char));
    int cnt = 0;
    while ((cnt = fread(buffor, sizeof(char), buffor_size, file1))) {

        fwrite(buffor, sizeof(char), cnt, file2);

    }
}

int main() {
    FILE * f1 = fopen("plik1", "r");
    FILE * f2 = fopen("plik2", "w");
    copy_lib(f1, f2, 16);
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    char buff1[100];
    scanf("%s", buff1);
    printf("|%s|", buff1);
}
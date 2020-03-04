#include <stdio.h>
#include <stdlib.h>

void a(int ** arr) {
	*arr = malloc(sizeof(int));
	*arr[0] = 2;
}

int main() {
	int * tab;
	a(&tab);
	printf("%d", tab[0]);

}

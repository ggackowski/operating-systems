#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char ** argv) {
	if (argc < 4) {
		printf("Too little arguments");
		return 1;
	}
	char * fifo_path = argv[1];
	char * file_path = argv[2];
	int buff_size = atoi(argv[3]);
	FILE * fifo = fopen(fifo_path, "r");
	FILE * res_file = fopen(file_path, "w");
	while (!feof(fifo)) {
		char * buffer = calloc(buff_size, sizeof(char));
		fgets(buffer, buff_size, fifo);
		fputs(buffer, res_file);
	}
}

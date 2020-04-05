#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>


int main(int argc, char ** argv) {
	srand(time(NULL));	
	if (argc < 4) {
		printf("too little arguments\n");
		return 1;
    	}
    	//sciezka do potoku nazwanego, siezka do pliku tekstowego, N - liczba znakow odczytywanych z pliku kazdorazowo
    	char * fifo_name = argv[1];
    	//mkfifo(fifo_name, 777);
    	char * file_path = argv[2];
    	int num_of_chars = atoi(argv[3]);

    	FILE * f = fopen(file_path, "r+");
    	FILE * fifo;
        fifo = fopen(fifo_name, "w");

    	while (!feof(f)) {
        	char * buffer = calloc(num_of_chars, sizeof(char));
        	fgets(buffer, num_of_chars, f);
        	pid_t pid = getpid();
			int wait = rand() % 4;
        	sleep(wait);
        	printf("#%d#%s", pid, buffer);
			fflush(stdout);
        	fprintf(fifo, "#%d#%s", pid, buffer);
    	}
		fclose(f);
		fclose(fifo);
}

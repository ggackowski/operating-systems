#include <stdio.h>
#include <sys/times.h>
#include <memory.h>
#include <unistd.h>
#include "diff_lib.h"
#include <dlfcn.h>

double time_difference(clock_t start, clock_t stop) {
	double time_diff = (double)(stop - start) / sysconf(_SC_CLK_TCK);
	return time_diff;
}

void parse_filenames(char * input, char ** filename1, char ** filename2) {
	char * divisor = strchr(input, ':');

	*filename1 = calloc((int)((divisor - input) + 1) , sizeof(char));
	strncpy(*filename1, input, (int)(divisor - input));
	(*filename1)[(int)(divisor - input)] = '\0';

	*filename2 = calloc((int)(strlen(input) - strlen(*filename1)) , sizeof(char));
	strcpy(*filename2, divisor + 1);
	(*filename2)[(int)(strlen(input) - strlen(*filename1) - 1)] = '\0';
}

int main(int argc, char ** argv) {

    void * handle = dlopen("./diff_lib.so", RTLD_LAZY);
	
	int * sizes = NULL;
	char *** block_array = NULL;
	char *** sequence1 = NULL;
	int seq1act_size = 0;
	int num_of_blocks = 0;
	char * filename1 = NULL;
	char * filename2 = NULL;
	int i = 1;
	int j = 0;
	int k = 0;

	struct tms * tms_start = calloc(1, sizeof(struct tms));
	struct tms * tms_stop = calloc(1, sizeof(struct tms));
	clock_t clock_start = 0;
	clock_t clock_stop = times(tms_stop);

    void (*create_table) (int, int **, char ****) = dlsym(handle, "create_table");
    void (*create_sequence) (int, char ****, int *) = dlsym(handle, "create_sequence");
    void (*add_to_sequence) (char ***, int *, char *, char *) = dlsym(handle, "add_to_sequence");
    int (*execute_sequence_at) (char ***, int, char ***, int, int *) = dlsym(handle, "execute_sequence_at");
    void (*remove_operation) (char ***, int, int, int *) = dlsym(handle, "remove_operation");
    void (*remove_block) (char ***, int, int *) = dlsym(handle, "remove_block");

	while (i < argc) {
		printf("Wykonanie: \"%s\"\n", argv[i]);
		clock_start = times(tms_start);
		
		if (strcmp(argv[i], "create_table") == 0) {
			(*create_table) (atoi(argv[i + 1]), &sizes, &block_array);
			num_of_blocks = atoi(argv[i + 1]);
			i++;
		}
		else if (strcmp(argv[i], "compare_pairs") == 0) {
			i++;
			j = i;	
			while (j < argc && (strcmp(argv[j], "remove_block") && strcmp(argv[j], "remove_operation") && strcmp(argv[j], "create_table"))) {
				j++;

			}
			(*create_sequence) (j - i, &sequence1, &seq1act_size);
			while (i < argc && (strcmp(argv[i], "remove_block") && strcmp(argv[i], "remove_operation") && strcmp(argv[i], "create_table"))) {
				
				parse_filenames(argv[i], &filename1, &filename2);
				(*add_to_sequence) (sequence1, &seq1act_size, filename1, filename2);
				++i;
			}	
			for (k = 0; k < seq1act_size; ++k) (*execute_sequence_at) (sequence1, k, block_array, num_of_blocks, sizes);
			i--;
		}
		else if(strcmp(argv[i], "remove_block") == 0) {
			(*remove_block) (block_array, atoi(argv[i + 1]), sizes);
			i++;

		}
		else if(strcmp(argv[i], "remove_operation") == 0) {
			(*remove_operation) (block_array, atoi(argv[i + 1]), atoi(argv[i + 2]), sizes);
			i += 2;
		}
	
		else {
			printf("niepoprawny argument %s\n", argv[i]);
		}

		clock_stop = times(tms_stop);
		printf("real time: %f\n", time_difference(clock_start, clock_stop));
		printf("system time: %f\n", time_difference(tms_start->tms_stime, tms_stop->tms_stime));
		printf("user time: %f\n\n\n", time_difference(tms_start->tms_utime, tms_stop->tms_utime));
		i++;	
	}
    dlclose(handle);
}
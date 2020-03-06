#include <stdio.h>
#include <sys/times.h>
#include "diff_lib.c"



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
	
	int * sizes = NULL;
	char *** block_array = NULL;
	char *** sequence1 = NULL;
	int seq1act_size = 0;
	int num_of_blocks = 2;
	char * filename1 = NULL;
	char * filename2 = NULL;
	int i = 1;
	int j = 0;
	int k = 0;

	while (i < argc) {
	
		if (strcmp(argv[i], "create_table") == 0) {
			create_table(atoi(argv[i + 1]), &sizes, &block_array);
			i++;
		}
		else if (strcmp(argv[i], "compare_pairs") == 0) {
			i++;
			j = i;	
			while (j < argc && (strcmp(argv[j], "remove_block") && strcmp(argv[j], "remove_operation") && strcmp(argv[j], "create_table"))) {
				j++;

			}

			create_sequence(j, &sequence1, &seq1act_size);
			while (i < argc && (strcmp(argv[i], "remove_block") && strcmp(argv[i], "remove_operation") && strcmp(argv[i], "create_table"))) {
				parse_filenames(argv[i], &filename1, &filename2);
				add_to_sequence(sequence1, &seq1act_size, filename1, filename2);
				++i;
			}	
			for (k = 0; k < seq1act_size; ++k) execute_sequence_at(sequence1, k, block_array, num_of_blocks, sizes);
			continue;
		}
		else if(strcmp(argv[i], "remove_block") == 0) {
			remove_block(block_array, atoi(argv[i + 1]), sizes);
			i++;

		}
		else if(strcmp(argv[i], "remove_operation") == 0) {
			remove_operation(block_array, atoi(argv[i + 1]), atoi(argv[i + 2]), sizes);
			i += 2;
		}
	
		else {
			printf("niepoprawny argument %s\n", argv[i]);
		}
		i++;	
	}
}

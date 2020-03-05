#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

void divide_string(char * str, char *** operations_arr, int * sizes, int index) {
	int i;
	int j;
	int cnt = 1;
	int bck = 0;
	for (i = 0; str[i] != '\0'; ++i) {
		if (str[i] == '\n' && isdigit(str[i+1])) {
			cnt++;
		}
	}
	
	(*operations_arr) = calloc(cnt, sizeof(char*));
	sizes[index] = cnt;

	int * positions = calloc((cnt - 1),  sizeof(int));
	j = 0;
	for (i = 0; str[i] != '\0'; ++i) {
		if (str[i] == '\n' && isdigit(str[i + 1])) {
				positions[j++] = i;
		}
	}
	for (i = 0; i < cnt - 1; ++i) {
		(*operations_arr)[i] = calloc((positions[i] - bck + 1)	, sizeof(char));
		strncpy((*operations_arr)[i], str + bck, positions[i] - bck);
		(*operations_arr)[i][positions[i] - bck] = '\0';
		bck = positions[i] + 1;
		printf("%s\n", (*operations_arr)[i]);
	}

	(*operations_arr)[i] = calloc((strlen(str) - bck + 1) , sizeof(char));
	strncpy((*operations_arr)[i], str + bck, strlen(str) - bck);
	(*operations_arr)[i][strlen(str) - bck] = '\0';
	printf("%s\n", (*operations_arr)[i]);
}	

void create_table(int size, int ** sizes, char **** blocks) {
	(*sizes) = malloc(sizeof(int) * size);
	int i;
	for (i = 0; i < size; ++i)
		(*sizes)[i] = 0;
	if (!(*blocks = calloc(size , sizeof(char **)))) {
		printf("error with creating blocks table");
	}
}

void create_sequence(int count, char **** sequence, int * length) {
	*sequence = calloc(2 , sizeof(char ***));
	(*sequence)[0] = calloc(count , sizeof(char **));
	(*sequence)[1] = calloc(count , sizeof(char **));
	*length = 0;
}

void add_to_sequence(char *** sequence, int * length, char * file1, char * file2) {
	sequence[0][*length] = calloc(strlen(file1) , sizeof(char));
	strcpy(sequence[0][*length], file1);

	sequence[1][*length] = calloc(strlen(file2) , sizeof(char));
	strcpy(sequence[1][*length], file2);

	(*length)++;
}

void remove_sequence(char **** sequence, int length) {
	int i;
	for (i = 0; i < length; ++i) {
		free((*sequence)[0][i]);
		free((*sequence)[1][i]);
	}
	free((*sequence)[0]);
	free((*sequence)[1]);
	free(*sequence);
}

void execute_sequence_at(char *** sequence, int index, char *** blocks_table, int size, int * sizes) {
	int i;

	for (i = 0; i < size; ++i) {
		if (sizes[i] == 0) {
			char * command = calloc((strlen(sequence[0][index]) + strlen(sequence[1][index]) + 13) , sizeof(char));
			command[0] = '\0';
			strcat(command, "diff ");
			strcat(command, sequence[0][index]);
			strcat(command, " ");
			strcat(command, sequence[1][index]);
			strcat(command, " > tmp");
			printf("\n\n%s\n\n", command);
			char t[200];
			system(command);
			FILE * f = fopen("tmp", "r");
			fread(t, 200, 1, f);
			divide_string(t, &blocks_table[i], sizes, i);
			fclose(f);
			system("rm tmp");
		}
	}
	//error no free space in block table
}






int create_and_parse_pair(int i, char ** argv, int argc, char *** block_array, int index, int * sizes) {
	if (++i == argc) return i; 
	char * divisor = NULL;
	while ((divisor = strchr(argv[i], ':'))) {
		char * str = calloc(strlen(argv[i]) , sizeof(char));
		strcpy(str, argv[i]);
		char * file_name1 = calloc((int)((divisor - argv[i]) + 1) , sizeof(char));
		strncpy(file_name1, str, (int)(divisor - argv[i]));
		file_name1[(int)(divisor-argv[i])] = '\0';
		char * file_name2 = calloc((int)(strlen(argv[i]) - strlen(file_name1)) , sizeof(char));
		strcpy(file_name2, divisor + 1);
		char * command = calloc((strlen(file_name1) + strlen(file_name2) + 7) , sizeof(char));
		command[0] = '\0';
		strcat(command, "diff ");
		strcat(command, file_name1);
		strcat(command, " ");
		strcat(command, file_name2);
		char t[200];
		FILE * f = popen(command, "r");
		fread(t, 200, 1, f);
		divide_string(t, &block_array[index], sizes, index);

		if (++i == argc) break;
	}
	return i - 1;
}

void remove_operation(char *** block_array, int block_number, int operation_number, int * sizes) {
	sizes[block_number]--;
	free(block_array[block_number][operation_number]);
}

void remove_block(char *** block_array, int block_number, int * sizes) {
	int i;
	for (i = 0; i < sizes[block_number]; ++i) {
		remove_operation(block_array, block_number, i, sizes);
	}
	sizes[block_number] = 0;
	free(block_array[block_number]);
}

int operations_in_block(int block_number, int * sizes) {
	return sizes[block_number];
}

int main(int argc, char ** argv) {
	int * sizes = NULL;
	char *** block_array = NULL;
	int seq1max_size = 1;
	char *** sequence1 = NULL;
	int seq1act_size = 0;
	int num_of_blocks = 2;

	create_table(num_of_blocks, &sizes, &block_array);
	create_sequence(seq1max_size, &sequence1, &seq1act_size);
	add_to_sequence(sequence1, &seq1act_size, "a.txt", "b.txt");
	add_to_sequence(sequence1, &seq1act_size, "a.txt", "b.txt");

	execute_sequence_at(sequence1, 0, block_array, num_of_blocks, sizes);
	execute_sequence_at(sequence1, 1, block_array, num_of_blocks, sizes);
	printf("%d\n", operations_in_block(0, sizes));

	remove_operation(block_array, 0, 1, sizes);
	printf("%d\n", operations_in_block(0, sizes));

	remove_block(block_array, 0, sizes);
	printf("%d", operations_in_block(0, sizes));


	
	// checking the parameters
	// 
}
/*
	while (i < argc) {
	
		if (strcmp(argv[i], "create_table") == 0) {
			block_array = create_table(atoi(argv[++i]), &sizes);
		}
		else if (strcmp(argv[i], "compare_pairs") == 0) {
			i = create_and_parse_pair(i, argv, argc, block_array, index, sizes);
			index++;
		}
		else if(strcmp(argv[i], "remove_block") == 0) {
			remove_block(block_array, atoi(argv[++i]), sizes);
		}
		else if(strcmp(argv[i], "remove_operation") == 0) {
			remove_operation(&block_array[atoi(argv[i + 1])], atoi(argv[i + 2]));
			i += 2;
		}
	
		else {
			printf("niepoprawny argument %s\n", argv[i]);
		}
			i++;	
	}

*/

#include "diff_lib.h"

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
	malloc(cnt * sizeof(char *));
	(*operations_arr) = calloc(cnt , sizeof(char*));
	sizes[index] = cnt;

	int * positions = calloc((cnt - 1) ,  sizeof(int));
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

		//printf("\n\n%s\n", (*operations_arr)[i]);
	}

	(*operations_arr)[i] = calloc((strlen(str) - bck + 1) , sizeof(char));
	strncpy((*operations_arr)[i], str + bck, strlen(str) - bck);
	(*operations_arr)[i][strlen(str) - bck] = '\0';
	//printf("\n\n%s\n", (*operations_arr)[i]);
	free(positions);
}	

void create_table(int size, int ** sizes, char **** blocks) {
	(*sizes) = calloc(sizeof(int) , size);
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

int execute_sequence_at(char *** sequence, int index, char *** blocks_table, int size, int * sizes) {
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
			char * t = NULL;
			if(!(t = calloc(1000000, sizeof(char)))) {
				printf("ERROR");
			}
			system(command);
			FILE * f = NULL;
			if (!(f = fopen("tmp", "r"))) {
			}
			
			if (fread(t, 1000000, 1, f)) {
			}
			divide_string(t, &blocks_table[i], sizes, i);
			fclose(f);
			system("rm tmp");
			free(t);
			free(command);
			return i;
		}
	}
	return -1;
	//error no free space in block table
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


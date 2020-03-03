#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char ** create_table(int size) {
	char ** blocks = NULL;
	if ((blocks = malloc(size * sizeof(char *))))
		return blocks;
	else {
		printf("error with creating blocks table");
		return NULL;
	}
}

int parse_files(int i, char ** argv, int argc) {
	if (++i == argc) return i; 
	char * divisor = NULL;
	while ((divisor = strchr(argv[i], ':'))) {
		printf("%s\n", argv[i]);
		char * str = malloc(strlen(argv[i]) * sizeof(char));
		strcpy(str, argv[i]);
		printf("%s\n", str);
		printf("dlugosc: %d\n", (int)(divisor - argv[i]));
		char * file_name1 = malloc((int)((divisor - argv[i]) + 1) * sizeof(char));
		strncpy(file_name1, str, (int)(divisor - argv[i]));
		file_name1[(int)(divisor-argv[i])] = '\0';
		//fflush(stdout);
		char * file_name2 = malloc((int)(strlen(argv[i]) - strlen(file_name1)) * sizeof(char));
		strcpy(file_name2, divisor + 1);
		printf("nazwa pliku 1: %s\n", file_name1);
		printf("nazwa pliku 2: %s\n", file_name2);
		char * command = malloc((strlen(file_name1) + strlen(file_name2) + 7) * sizeof(char));
		command[0] = '\0';
		strcat(command, "diff ");
		strcat(command, file_name1);
		strcat(command, " ");
		strcat(command, file_name2);
		printf("%s\n", command);
		system(command);
		if (++i == argc) break;

	}
	return i;
}

int main(int argc, char ** argv) {
	printf("%d\n ", argc);
	printf("hello\n");
	int i;
	for (i = 0; i < argc; ++i) 
		printf("%s\n", argv[i]);

	char ** block_array = NULL;

	// checking the parameters
	// 
	i = 1;
	while (i < argc) {
	printf("%d\n", i);
	fflush(stdout);
	if (strcmp(argv[i], "create_table") == 0) {
		printf("robie tabele\n");
		block_array = create_table(atoi(argv[i + 1]));
		++i;
	}
	else if (strcmp(argv[i], "compare_pairs") == 0) {
		fflush(stdout);
		printf("afsdfsdf");
		i = parse_files(i, argv, argc);
	}
	
	else {
		printf("niepoprawny argument\n");
	}
		i++;
	}
}

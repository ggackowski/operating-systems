#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#ifndef _DIFF_LIB_H_
#define _DIFF_LIB_H_

void divide_string(char * str, char *** operations_arr, int * sizes, int index);
void create_table(int size, int ** sizes, char **** blocks);
void create_sequence(int count, char **** sequence, int * length);
void add_to_sequence(char *** sequence, int * length, char * file1, char * file2);
void remove_sequence(char **** sequence, int length);
int execute_sequence_at(char *** sequence, int index, char *** blocks_table, int size, int * sizes);
void remove_operation(char *** block_array, int block_number, int operation_number, int * sizes);
void remove_block(char *** block_array, int block_number, int * sizes);
int operations_in_block(int block_number, int * sizes); 

#endif
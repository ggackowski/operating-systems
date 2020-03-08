all: program_static

program_static: program.o diff_lib.o
	gcc -Wall -g program.o diff_lib.o -o program

program_shared: diff_lib.so program_s.o
	gcc -shared -g -o diff_lib.so program.o

program_static_test1: program_static
	./program create_table 1 compare_pairs test1a:test1b
program_shared_test1: program_shared
	./program create_table 1 compare_pairs test1a:test1b

program.o: program.c
	gcc -c program.c 

program_s.o: program.c
	gcc -c -fpic program.c

diff_lib.o: diff_lib.c
	gcc -c diff_lib.c

diff_lib_s.o: diff_lib.c
	gcc -c -Wall -fpic diff_lib.c

diff_lib.so: diff_lib_s.o
	gcc -shared -o diff_lib.so diff_lib.o


clean:
	rm *o
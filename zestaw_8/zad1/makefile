all: main.c
	gcc -Wall -pthread main.c -lm -o main

test:
	./main 4 interleaved mountain.ascii.pgm output.ascii.pgm

clean:
	rm main
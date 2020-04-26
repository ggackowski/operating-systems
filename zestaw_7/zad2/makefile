all:	main.c maker.c sender.c packer.c
	gcc -Wall main.c stuff.c -o main -lrt -pthread
	gcc -Wall maker.c stuff.c -o maker -lrt -pthread
	gcc -Wall packer.c stuff.c -o packer -lrt -pthread
	gcc -Wall sender.c stuff.c -o sender -lrt -pthread
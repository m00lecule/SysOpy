CC = gcc -Wall -std=gnu11

all: main.o filehandlerlib
	$(CC) main.o libfilehandler.a -o main

main.o:
	$(CC) -c main.c -o main.o

filehandlerlib:
	$(CC) -c filehandler.c -o filehandler.o
	ar rcs libfilehandler.a filehandler.o

clean:
	rm -f *.o *.a main

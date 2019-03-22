CC = gcc -Wall -std=gnu11

all: main.o monitorlib
	$(CC) main.o monitorlib.a -o main

monitorlib:
	$(CC) -c monitor.c -o monitor.o
	ar rcs monitorlib.a monitor.o

main.o:
	$(CC) -c main.c -o main.o

clean:
	rm -f main *.o *.a

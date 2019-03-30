CC = gcc -Wall -std=gnu11 -g

all: main.o monitorlib
	$(CC) main.o monitorlib.a -o monitor

monitorlib:
	$(CC) -c monitor.c -o monitor.o
	ar rcs monitorlib.a monitor.o

tester.o:
	$(CC) -c tester.c -o tester.o

tester: tester.o
	$(CC) tester.o -o tester

main.o:
	$(CC) -c main.c -o main.o

clean:
	rm -f monitor *.o *.a tester main

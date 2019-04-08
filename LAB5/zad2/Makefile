CC = gcc -Wall -std=gnu99

all: master slave

master: master.c
	$(CC) master.c -o master

slace: slave.c
	$(CC) slave.c -o slave

clean:
	rm -f *.o master slave

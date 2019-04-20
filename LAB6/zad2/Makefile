CC = gcc -Wall -std=gnu99


all: server client

server:
	$(CC) server.c -o server -lrt

client: client.c
	$(CC) client.c -o client -lrt

clean:
	rm -f server client *.o

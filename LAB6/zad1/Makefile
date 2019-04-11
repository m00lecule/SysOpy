CC = gcc -Wall -std=gnu99


all: server client

server:
	$(CC) server.c -o server

client: client.c
	$(CC) client.c -o client

clean:
	rm -f server client *.o

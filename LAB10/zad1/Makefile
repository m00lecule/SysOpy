CC = gcc -Wall -std=gnu99

all: server client


server:
	$(CC) server.c -o server -lpthread


client:
	$(CC) client.c -o client -lpthread


clean:
	rm -rf client server

CC = gcc -Wall -std=gnu99

all: catcher sender

catcher: catcher.c
	$(CC) catcher.c -o catcher

sender: sender.c
	$(CC) sender.c -o sender

clean:
	rm -f *.o catcher sender

CC = gcc -Wall -std=gnu99
all: loader trucker worker

trucker:
	$(CC) trucker.c -o trucker -lpthread -lrt

loader:
	$(CC) loader.c -o loader -lpthread -lrt

worker:
	$(CC) worker.c -o worker -lpthread -lrt

clean:
	rm -rf loader worker trucker *.o

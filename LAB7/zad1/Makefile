CC = gcc -Wall -std=gnu99
all: loader trucker worker
#
# main:
# 	$(CC) main.c -o main -lpthread -lrt

trucker:
	$(CC) trucker.c -o trucker -lpthread -lrt

loader:
	$(CC) loader.c -o loader -lpthread -lrt

worker:
	$(CC) worker.c -o worker -lpthread -lrt

clean:
	rm -rf loader proces1 proces2 worker trucker *.o

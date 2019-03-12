CC = gcc -Wall -std=gnu11

all: static shared
	rm *.o

static:
	$(CC) -c findResultLib.c -o FindResultLib.o
	ar rcs libfindresult.a FindResultLib.o

shared:
	$(CC) -fPIC -c findResultLib.c -o FindResultLib1.o
	$(CC) -shared -fPIC -o libfindresult.so FindResultLib1.o

clean:
	rm -f *.o *.a *.so

CC = gcc -Wall -std=gnu11
ARGS = create_table 10 search_directory /home \\*.txt add_to_memory remove_block 0 add_to_memory add_to_memory search_directory /home/Dokumenty/Programy/SysOpy/ main.c add_to_memory add_to_memory remove_block 1 remove_block 2 remove_block 3 search_directory / \\* add_to_memory remove_block 1
LOG = zad2a.txt

test: all
	./main $(ARGS) >$(LOG)
	make clean

all: main.o static
	$(CC) main.o libfindresult.a -o main

main.o:
	$(CC) -c main.c -o main.o

static:
	$(CC) -c findResultLib.c -o FindResultLib.o
	ar rcs libfindresult.a FindResultLib.o

clean:
	rm -f *.o *.tmp *.a *.so main

CC = gcc -Wall -std=gnu11

ARGS = create_table 10 search_directory /home \\*.txt add_to_memory remove_block 0 add_to_memory add_to_memory search_directory /home/Dokumenty/Programy/SysOpy/ main.c add_to_memory add_to_memory remove_block 1 remove_block 2 remove_block 3 search_directory / \\* add_to_memory remove_block 1
LOG_A = zad3a.txt
LOG_B = zad3b.txt


all_static: main.o static
	$(CC) main.o libfindresult.a -o main

all_shared: main.o shared
	$(CC) -o main main.o -L. libfindresult.so -I .

all_dynamic: shared
	$(CC) -c main.c -o main.o -D DYNAMIC="libfindresult.so"
	$(CC) -o main main.o -ldl -I .

shared:
	$(CC) -fPIC -c findResultLib.c -o FindResultLib1.o
	$(CC) -shared -fPIC -o libfindresult.so FindResultLib1.o

main.o:
	$(CC) -c main.c -o main.o

static:
	$(CC) -c findResultLib.c -o FindResultLib.o
	ar rcs libfindresult.a FindResultLib.o

clean:
	rm -f *.o *.a *.so *.tmp main

test:
		rm -f $(LOG_A)
		make clean
		make all_static
		echo "\n STATIC \n" >> $(LOG_A)
		./main $(ARGS) >> $(LOG_A)
		make clean
		make all_dynamic
		echo "\n DYNAMIC \n" >> $(LOG_A)
		./main $(ARGS) >> $(LOG_A)
		make clean
		make all_shared
		echo "\n SHARED \n" >> $(LOG_A)
		./main $(ARGS) >> $(LOG_A)
		make clean

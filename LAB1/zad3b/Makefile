CC = gcc -Wall -std=gnu11

ARGS = a+ create_table 10 search_directory /home \\*.txt home add_to_memory home add_to_memory home add_to_memory home search_directory /home/michald \\* homemichal remove_block 0 remove_block 1 add_to_memory homemichal add_to_memory homemichal add_to_memory homemichal search_directory /home/michald/Dokumenty/Programy/SysOpy \\* sysopy  remove_block 2 remove_block 3 add_to_memory sysopy add_to_memory sysopy add_to_memory sysopy remove_block 0 remove_block 1
LOG = results3b.txt

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
		rm -f $(LOG)
		@for type in static shared dynamic; do \
			for number in 0 1 2 3 s; do \
				make clean ; \
				make all_$$type OPT=O$$number ; \
				echo "\n $$type O$$number \n" >> $(LOG) ;\
				echo "\n $$type O$$number \n" ;\
				./main $(LOG) $(ARGS) ;\
				echo "\n";\
				done	\
			done
		make clean

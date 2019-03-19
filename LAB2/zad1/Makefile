CC = gcc -Wall -std=gnu11
LOG = wyniki.txt

all: main.o filehandlerlib
	$(CC) main.o libfilehandler.a -o main

main.o:
	$(CC) -c main.c -o main.o

filehandlerlib:
	$(CC) -c filehandler.c -o filehandler.o
	ar rcs libfilehandler.a filehandler.o

clean:
	rm -f *.o *.a main

test:
	make clean
	make all
	rm -f wyniki.txt
	@for format in "1 6000 3000" \
								"4 5000 3000" \
	 							"512 5000 2000" \
								"1024 4000 2000" \
								"4096 3000 2000" \
								"8192 3000 2000 "; do \
		set -- $$format;\
		./main generate origin_1 $$2 $$1 ;\
		./main generate origin_2 $$3 $$1 ;\
		for type in lib sys; do \
		echo "\n" >> $(LOG) ;\
		 echo "$$type \n ">> $(LOG) ;\
		 ./main copy origin_1 origin_1_$$type $$2 $$1 $$type >> $(LOG);\
		 ./main copy origin_2 origin_2_$$type $$3 $$1 $$type >> $(LOG);\
		 ./main sort origin_1_$$type $$2 $$1 $$type >> $(LOG);\
		 ./main sort origin_2_$$type $$3 $$1 $$type >> $(LOG);\
		 rm origin_1_$$type;\
		 rm origin_2_$$type;\
			done	;\
	 done
	rm -f origin_1 origin_2
	make clean

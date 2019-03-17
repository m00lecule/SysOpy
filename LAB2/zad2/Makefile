SHELL=/bin/bash
CC = gcc -Wall -std=gnu11
DIR = test_directory
DIR_LIST = $(DIR)/{pictures/{2018,2019},documents/{public,private,shared,protected},movie/{titanic,lotr,jaws},music/{rock,pop,indie},gallery/{birthday,christmass,funeral}}
FILE_LIST = $(DIR)/pictures/2018/zdj1.png $(DIR)/pictures/2019/zdj2.png $(DIR)/documents/public/book.pdf $(DIR)/documents/public/book2.pdf $(DIR)/documents/public/book3.pdf


all_stat: stat_directorylib
	$(CC) -c main.c -o main.o -D STAT
	$(CC) main.o stat_directorylib.a -o main

all_nftw: main.o nftw_directorylib
	$(CC) main.o nftw_directorylib.a -o main

main.o:
	$(CC) -c main.c -o main.o

stat_directorylib:
	$(CC) -c stat_directory.c -o stat_directory.o
	ar rcs stat_directorylib.a stat_directory.o

nftw_directorylib:
	$(CC) -c nftw_directory.c -o nftw_directory.o
	ar rcs nftw_directorylib.a nftw_directory.o

clean:
	rm -f *.o *.a main

test:
	mkdir -p $(DIR_LIST)
	touch $(FILE_LIST)
	ln -s / $(DIR)/music/rock/loop
	ln -s / $(DIR)/gallery/birthday/loop2
	@for type in stat nftw; do \
		make clean ;\
		make all_$$type ;\
		echo " $$type " ;\
		./main test_directory "2019 3 12 20:20" "<" ;\
	done
	make clean
	rm -rf $(DIR)

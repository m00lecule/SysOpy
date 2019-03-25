SHELL=/bin/bash
CC = gcc -Wall -std=gnu11 -g
DIR = test_directory
DIR_LIST = $(DIR)/{pictures/{2018,2019},documents/{public,private,shared,protected},movie/{titanic,lotr,jaws},music/{rock,pop,indie},gallery/{birthday,christmass,funeral}}
FILE_LIST = $(DIR)/pictures/2018/zdj1.png $(DIR)/pictures/2019/zdj2.png $(DIR)/documents/public/book.pdf $(DIR)/documents/public/book2.pdf $(DIR)/documents/public/book3.pdf


all: main.o stat_directorylib
	$(CC) main.o stat_directorylib.a -o main

main.o:
	$(CC) -c main.c -o main.o

stat_directorylib:
	$(CC) -c stat_directory.c -o stat_directory.o
	ar rcs stat_directorylib.a stat_directory.o

clean:
	rm -f *.o *.a main
	rm -rf $(DIR)

test:
	mkdir -p $(DIR_LIST)
	touch $(FILE_LIST)
	make clean
	make all
	./main "."
	rm -r $(DIR)
	make clean

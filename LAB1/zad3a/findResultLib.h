#ifndef FINDRESULTLIB
#define FINDRESULTLIB

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

typedef struct MemoryArray{
  char** blk;
  int size;
}MemoryArray;


//konstruktor struktury przechowywującej wyniki zapytań w pamięci
struct MemoryArray* init_memory_array(int);
//setter nazwy pliku
void set_filename(const char *);
//setter nazwy katalogu
void set_directory(const char *);

//wczytywanie zawartości pliku tymczasowego do pamięci RAM
int fetch_from_tmp(struct MemoryArray*);

//napisanie obecenego pliku tymczasowego nowym zapytaniem
void exec_find();

//zwolenie bloku pamięci w strukturze MemoryArray pod konkretnym indeksem
void reset_index(struct MemoryArray* ,int);

//zwolnienie pamięci
void free_memory(struct MemoryArray*);

#endif

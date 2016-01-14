#ifndef IMALLOC_H
#define IMALLOC_H
#include<unistd.h>
#include<sys/types.h>
void *Imalloc(size_t size);
void *Icalloc(size_t number,size_t size);
void Ifree(void *p);
void *Irealloc(void *p,size_t size);
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "ints.h"
#include "io.h"

uint8_t * loadslice(FILE * fd, size_t start, size_t len)
{
    uint8_t * buf = malloc(len);
    fseek(fd, start, SEEK_SET);
    fread(buf, len, 1, fd);
    return buf;
}

uint8_t * loadfile(const char * name, size_t * size)
{
    FILE * fin = fopen(name, "rb");
    if (!fin) return NULL;
    
    fseek(fin, 0, SEEK_END);
    *size = ftell(fin);
    rewind(fin);
    
    uint8_t * buf = loadslice(fin, 0, *size);
    fclose(fin);
    return buf;
}

#ifndef ENCELADUS_IO_H
#define ENCELADUS_IO_H

uint8_t * loadslice(FILE * fd, size_t start, size_t len);
uint8_t * loadfile(const char * name, size_t * size);

#endif

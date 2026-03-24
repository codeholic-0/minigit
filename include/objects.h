#ifndef OBJECTS_H
#define OBJECTS_H

#include<stddef.h>

char *write_object(const char *type, const void *data, size_t size);
char *hash_object(const char *filepath);
char *read_object(const char *hash, size_t *out_size);

#endif

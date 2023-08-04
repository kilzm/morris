#ifndef VEC_H
#define VEC_H

#include <sys/types.h>

typedef unsigned char type_t;
typedef void* vec;

#define vec_pushbacks(vectorAdress)                                     \
    ((typeof(*vectorAdress))(                                       \
        __pushback((vec*) vectorAdress, sizeof(**vectorAdress))))

#define vec_pushback(vectorAdress, value) (*vec_pushbacks(vectorAdress) = value)

vec vec_create(void);
vec vec_with_cap(size_t capacity, size_t type_size);

void vec_free(vec vector);
void *__pushback(vec *vectorAdress, type_t type_size);

size_t vec_get_alloc(vec vector);
size_t vec_get_length(vec vector);

#endif /* VEC_H */
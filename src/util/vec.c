#include <stdlib.h>
#include <stdbool.h>

#include "vec.h"

typedef struct vecdata {
    size_t alloc;
    size_t length;
    char buffer[];
} vecdata_t;

vec vec_create(void)
{
    vecdata_t *vector = malloc(sizeof *vector);
    vector->alloc = 0;
    vector->length = 0;
    return &vector->buffer;
}

vec vec_with_cap(size_t capacity, size_t type_size)
{
    vecdata_t *vector = malloc(sizeof *vector + capacity * type_size);
    vector->alloc = capacity;
    vector->length = 0;
    return &vector->buffer;
}

vecdata_t *vec_get(vec vector)
{
    return &((vecdata_t*) vector)[-1];
}

size_t vec_get_alloc(vec vector)
{
    return vec_get(vector)->alloc;
}

size_t vec_get_length(vec vector)
{
    return vec_get(vector)->length;
}

void vec_free(vec vector)
{
    free(vec_get(vector));
}

vecdata_t *vector_realloc(vecdata_t *vector, size_t type_size)
{
    size_t updatedAlloc = (vector->alloc == 0) ? 1 : vector->alloc * 2;
    vecdata_t *updatedvecdata_t = realloc(vector, sizeof *updatedvecdata_t + updatedAlloc * type_size);
    updatedvecdata_t->alloc = updatedAlloc;
    return updatedvecdata_t;
}

bool vec_has_space(vecdata_t *vector)
{
    return vector->alloc - vector->length > 0;
}

void *__pushback(vec *vectorAdress, type_t type_size)
{
    vecdata_t *vector = vec_get(*vectorAdress);

    if (!vec_has_space(vector)) {
        vector = vector_realloc(vector, type_size);
        *vectorAdress = vector->buffer;
    }

    return (void*) &vector->buffer[type_size*vector->length++];
}

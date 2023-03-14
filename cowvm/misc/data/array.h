#ifndef ARRAY_H_INCLUDED
#define ARRAY_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include "memory.h"

typedef struct {
    void **data;
    size_t size;
    size_t capacity;
} Array;

static void array_free(Array *array) {
    cow_free(array->data);
}

static void array_add(Array *array, void *element) {
    if (array->size + 1 > array->capacity) {
        ++array->capacity;
        array->capacity *= 2;
        array->data = (void **) cow_realloc(array->data, array->capacity * sizeof(void *));
    }

    array->data[array->size] = element;
    ++array->size;
}

static void array_pop(Array *array) {
    --array->size;
}

static void *array_get(Array *array, size_t index) {
    return array->data[index];
}

static void array_remove(Array *array, size_t index) {
    const size_t size_to_copy = array->size - index - 1;
    memmove(&array->data[index], &array->data[index + 1], size_to_copy * sizeof(void *));
    --array->size;
}


#endif // ARRAY_H_INCLUDED

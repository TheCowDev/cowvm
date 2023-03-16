#ifndef COWVM_MEMORY_H
#define COWVM_MEMORY_H

#include <stdio.h>
#include <stdlib.h>

static void *cow_alloc(size_t size) {
    return malloc(size);
}

static void *cow_calloc(size_t size) {
    return calloc(size, 1);
}

static void *cow_realloc(void *ptr, size_t size) {
    return realloc(ptr, size);
}

static void cow_free(void *ptr) {
    free(ptr);
}


#endif //COWVM_MEMORY_H

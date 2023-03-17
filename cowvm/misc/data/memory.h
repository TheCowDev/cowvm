#ifndef COWVM_MEMORY_H
#define COWVM_MEMORY_H

#include <stdio.h>
#include <stdlib.h>

#include <windows.h>

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

static void *cow_alloc_exec_mem(size_t size) {
    char *exec_buffer = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    return exec_buffer;
}


#endif //COWVM_MEMORY_H

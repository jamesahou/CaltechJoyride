#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "strarray.h"

strarray_t *strarray_init(size_t length) {
    strarray_t *arr = malloc(sizeof(strarray_t));
    arr->data = malloc(sizeof(char*) * length);
    arr->length = length;
    for(size_t i = 0; i < length; i++) {
        arr->data[i] = NULL;
    }
    return arr;
}

void strarray_free(strarray_t *arr) {
    for(size_t i  = 0; i < arr->length; i++) {
        free(arr->data[i]);
    }
    free(arr->data);
    free(arr);
}

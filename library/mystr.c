#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "strarray.h"
#include "mystr.h"

ssize_t mystr_indexof(const char *str, const char sep, size_t start) {
    for(size_t i = start; i < strlen(str); i++) {
        if(str[i] == sep) {
            return i;
        }
    }
    return -1;
}

size_t count_num_words(const char *str, const char sep) {
    size_t space = 1;
    size_t i = 0;
    size_t words = 0;
    while(str[i] != '\0') {
        if(space == 0 && str[i] == sep) {
            words++;
            space = 1;
        }
        else if(str[i] != sep) { space = 0; }
        i++;
    }
    if(space == 0) { words++; }
    return words;
}

strarray_t *mystr_split(const char *str, const char sep) {
    strarray_t *arr = strarray_init(count_num_words(str, sep));
    size_t arr_idx = 0;
    for(ssize_t i = 0; i < (ssize_t)strlen(str); i++) {
        if(str[i] != sep) {
            ssize_t idx = mystr_indexof(str, sep, i);
            if(idx != -1) {
                arr->data[arr_idx] = malloc(sizeof(char) * ((idx - i) + 1));
                size_t k = 0;
                while(i < idx) {
                    arr->data[arr_idx][k] = str[i];
                    i++;
                    k++;
                }
                arr->data[arr_idx][k] = '\0';
                arr_idx++;
            }
            else {
                ssize_t len = strlen(str);
                arr->data[arr_idx] = malloc(sizeof(char) * ((len - i) + 1));
                size_t k = 0;
                while(i < len) {
                    arr->data[arr_idx][k] = str[i];
                    i++;
                    k++;
                }
                arr->data[arr_idx][k] = '\0';
            }
        }
    }
    return arr;
}

#include "list.h"
#include "vector.h"
#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct list {
  void **arr;
  size_t size;
  size_t cap;
  free_func_t freer;
} list_t;

list_t *list_init(size_t initial_size, free_func_t freer) {
  list_t *v_list = malloc(sizeof(list_t));
  assert(v_list);
  v_list->arr = malloc(initial_size * sizeof(void *));
  assert(v_list->arr);
  v_list->size = 0;
  v_list->cap = initial_size;
  v_list->freer = freer;
  return v_list;
}

void list_free(list_t *list) {
  if (list->freer != NULL) {
    for (size_t i = 0; i < list->size; i++) {
      list->freer(list->arr[i]);
    }
  }
  free(list->arr);
  free(list);
}

size_t list_size(list_t *list) { return list->size; }

void *list_get(list_t *list, size_t index) {
  assert(index < list->size);
  return list->arr[index];
}

void resize(list_t *list) {
  // deals with the multiplying by 0 issues that can occur is cap is initially 0
  if (list->cap == 0) {
    list->cap = 1;
  }
  void **new_list = malloc(sizeof(void *) * (list->cap * 2));
  assert(new_list);
  for (size_t i = 0; i < list->size; i++) {
    new_list[i] = list->arr[i];
  }
  free(list->arr);
  list->arr = new_list;
  list->cap *= 2;
}

void list_add(list_t *list, void *value) {
  if (list->cap <= list->size) {
    resize(list);
  }
  assert(list->cap > list->size);
  assert(value);
  list->arr[list->size] = value;
  list->size++;
}

void *list_remove(list_t *list, size_t index) {
  assert(list->size > index);
  void *val = list->arr[index];
  for (size_t i = index; i < list->size - 1; i++) {
    list->arr[i] = list->arr[i + 1];
  }
  list->arr[list->size - 1] = NULL;
  list->size--;
  return val;
}

void list_sort(list_t *list, __compar_fn_t compare) {
  qsort(list->arr, list->size, sizeof(void *), compare);
}

void list_clear(list_t *list) {
  size_t num_items = list_size(list);
  
  if (num_items == 0) {
    return;
  } else {
    for (ssize_t i = num_items - 1; i >= 0; i--) {
      void *item = list_remove(list, i);
      list->freer(item);
    }
  }
}
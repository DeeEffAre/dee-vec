#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VEC_DEFAULT_CAP 64

typedef struct {
  void *data;
  size_t length;
  size_t capacity;
  size_t element_size;
  void (*free_function)(void *);
} vec_t;

typedef vec_t *Vec;

Vec vec_init(const void *data, size_t n, size_t element_size,
             void (*free_function)(void *)) {
  vec_t *vector = (vec_t *)malloc(sizeof(vec_t));
  if (!vector) {
    return NULL;
  }

  if (data && n > 0) {

    if (n < VEC_DEFAULT_CAP) {
      vector->data = malloc(element_size * VEC_DEFAULT_CAP);
      vector->capacity = VEC_DEFAULT_CAP;
    } else {
      vector->data = malloc(element_size * n);
      vector->capacity = n;
    }

    if (!vector->data) {
      fprintf(stderr, "Out of memory\n");
      return NULL;
    }

    vector->element_size = element_size;
    memcpy((vector->data), data, n * vector->element_size);

    vector->length = n;

  } else {
    vector->data = malloc(element_size * VEC_DEFAULT_CAP);
    vector->length = 0;
    vector->capacity = VEC_DEFAULT_CAP;
    vector->element_size = element_size;
  }

  vector->free_function = free_function;
  return vector;
}

// 0 on success
int vec_push(Vec vec, const void *data, size_t n) {
  if (vec->length + n > vec->capacity) {
    size_t new_capacity = vec->capacity * 2;
    while (new_capacity < vec->length + n) {
      new_capacity *= 2;
    }

    void *new_data_ptr = realloc(vec->data, new_capacity * vec->element_size);

    if (!new_data_ptr) {
      return -1;
    }

    vec->capacity = new_capacity;

    vec->data = new_data_ptr;
  }

  memcpy(((char *)(vec->data)) + (vec->length * vec->element_size), data,
         n * vec->element_size);

  vec->length += n;

  return 0;
}

// 0 on success
int vec_shrink(Vec vec) {
  if (vec->length == vec->capacity) {
    return -1;
  }

  void *new_data_ptr = realloc(vec->data, vec->length * vec->element_size);

  if (!new_data_ptr) {
    fprintf(stderr, "Out of memory\n");
    return -1;
  }

  vec->capacity = vec->length;
  vec->data = new_data_ptr;

  return 0;
}

void vec_pop(Vec vec, void *elem_buffer) {
  if (vec->length == 0) {
    elem_buffer = NULL;
    return;
  }

  if (vec->length == 0)
    return;

  void *elem = (char *)vec->data + (vec->length - 1) * vec->element_size;
  if (elem_buffer) {
    memcpy(elem_buffer, elem, vec->element_size);
  } else {
    if (vec->free_function) {
      vec->free_function(elem);
    }
  }
  vec->length--;
}

void *vec_get(Vec vec, size_t index) {
  if (index >= vec->length) {
    fprintf(stderr, "Index is out of bound\n");
    return NULL;
  }

  return ((char *)vec->data) + (index * vec->element_size);
}

void vec_free(Vec *vec) {
  if (vec == NULL)
    return;

  if ((*vec)->free_function) {
    for (size_t i = 0; i < (*vec)->length; ++i) {
      (*vec)->free_function(vec_get((*vec), i));
    }
  }

  free((*vec)->data);
  free(*vec);
  *vec = NULL;
}

void vec_clear(Vec vec) {
  if (vec->free_function) {
    for (size_t i = 0; i < vec->length; ++i) {
      vec->free_function(vec_get(vec, i));
    }
  }

  vec->length = 0;
}

size_t vec_length(Vec vec) { return vec->length; }
size_t vec_capacity(Vec vec) { return vec->capacity; }
size_t vec_size(Vec vec) { return vec->length * vec->element_size; }

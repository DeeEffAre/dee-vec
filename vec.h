#ifndef VEC_H
#define VEC_H

#include <stddef.h>

void *vec_init(const void *data, size_t n, size_t element_size,
               void (*free_function)(void *));
void *vec_push(void *vec, const void *data, size_t n);
void *vec_shrink(void *vec);
void vec_pop(void *vec, void *elem_buffer);
void *vec_get(void *vec, size_t index);
void vec_free(void **vec_ptr);
void vec_clear(void *vec);
size_t vec_length(void *vec);
size_t vec_capacity(void *vec);
size_t vec_size(void *vec);

#endif // !VEC_H

#ifndef VEC_IMPLEMENTATION

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VEC_DEFAULT_CAP 64
#define GET_VEC_HEADER(v)                                                      \
  ((vec_header_t *)((char *)(v) - offsetof(vec_header_t, data)))

typedef struct {
  size_t length;
  size_t capacity;
  size_t element_size;
  void (*free_function)(void *);
  char data[];
} vec_header_t;

/* Initializes a vector and returns a pointer to its data.
   `data`: initial elements to copy (optional, can be NULL)
   `n`: number of initial elements
   `element_size`: size of each element in bytes
   `free_function`: used for deep-freeing elements, provide NULL if not needed.
 */
void *vec_init(const void *data, size_t n, size_t element_size,
               void (*free_function)(void *)) {
  size_t allocation_capacity = VEC_DEFAULT_CAP;
  if (n > VEC_DEFAULT_CAP) {
    allocation_capacity = n;
  }

  vec_header_t *header = (vec_header_t *)malloc(
      sizeof(vec_header_t) + (allocation_capacity * element_size));

  if (!header) {
    return NULL;
  }

  if (data && n > 0) {
    header->capacity = allocation_capacity;

    header->element_size = element_size;
    memcpy((header->data), data, n * header->element_size);

    header->length = n;

  } else {
    header->length = 0;
    header->capacity = VEC_DEFAULT_CAP;
    header->element_size = element_size;
  }

  header->free_function = free_function;
  return header->data;
}

/* Pushes `n` new elements to the vector.
   Returns the (potentially new) pointer to the vector data.
   IMPORTANT: Always assign the return value back to your vector variable,
   as realloc may move the memory block. */
void *vec_push(void *vec, const void *data, size_t n) {
  if (!vec || !data || n == 0)
    return vec;

  vec_header_t *header = GET_VEC_HEADER(vec);

  if (header->length + n > header->capacity) {
    size_t new_capacity = header->capacity * 2;
    while (new_capacity < header->length + n) {
      new_capacity *= 2;
    }

    size_t new_size =
        sizeof(vec_header_t) + (new_capacity * header->element_size);

    vec_header_t *new_header = (vec_header_t *)realloc(header, new_size);

    // if fails return the old data
    if (!new_header) {
      return vec;
    }

    header = new_header;
    new_header->capacity = new_capacity;
  }

  memcpy(((char *)(header->data)) + (header->length * header->element_size),
         data, n * header->element_size);

  header->length += n;

  return header->data;
}

/* Shrinks vector capacity to match its current length.
   Returns the (potentially new) pointer to the vector data. */
void *vec_shrink(void *vec) {
  vec_header_t *header = GET_VEC_HEADER(vec);

  if (header->length == header->capacity) {
    return header->data;
  }

  vec_header_t *new_header = (vec_header_t *)realloc(
      header, sizeof(vec_header_t) + header->length * header->element_size);

  if (!new_header) {
    fprintf(stderr, "Out of memory\n");
    return header->data;
  }
  header = new_header;

  header->capacity = header->length;

  return header->data;
}

/* Pops the last element. If `elem_buffer` is not NULL, copies the element
   there. Otherwise, frees the element using `free_function` if provided. */
void vec_pop(void *vec, void *elem_buffer) {
  vec_header_t *header = GET_VEC_HEADER(vec);

  if (header->length == 0) {
    elem_buffer = NULL;
    return;
  }

  if (header->length == 0)
    return;

  void *elem =
      ((char *)(header->data)) + (header->length - 1) * header->element_size;
  if (elem_buffer) {
    memcpy(elem_buffer, elem, header->element_size);
  } else {
    if (header->free_function) {
      header->free_function(elem);
    }
  }
  header->length--;
}

/* Returns a generic pointer to the element at `index`. */
void *vec_get(void *vec, size_t index) {
  vec_header_t *header = GET_VEC_HEADER(vec);

  if (index >= header->length) {
    fprintf(stderr, "Index is out of bound\n");
    return NULL;
  }

  return ((char *)(header->data)) + (index * header->element_size);
}

/* Frees the entire vector and its elements if free_function provided. */
void vec_free(void **vec_ptr) {
  if (vec_ptr == NULL || *vec_ptr == NULL)
    return;

  vec_header_t *header = GET_VEC_HEADER(*vec_ptr);

  if (header->free_function) {
    for (size_t i = 0; i < header->length; ++i) {
      header->free_function(vec_get(header->data, i));
    }
  }

  free(header);
  *vec_ptr = NULL;
}

/* Clears the contents of the vector without freeing it */
void vec_clear(void *vec) {
  vec_header_t *header = GET_VEC_HEADER(vec);

  if (header->free_function) {
    for (size_t i = 0; i < header->length; ++i) {
      header->free_function(vec_get(header->data, i));
    }
  }

  header->length = 0;
}

/* Returns the length of the vector */
size_t vec_length(void *vec) { return GET_VEC_HEADER(vec)->length; }
/* Returns the capacity of the vector */
size_t vec_capacity(void *vec) { return GET_VEC_HEADER(vec)->capacity; }
/* Returns the total size in bytes of all elements */
size_t vec_size(void *vec) {
  return GET_VEC_HEADER(vec)->length * GET_VEC_HEADER(vec)->element_size;
}

#endif // !VEC_IMPLEMENTATION

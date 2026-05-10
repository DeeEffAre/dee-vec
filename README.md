# dee-vec
[std style single header](https://github.com/nothings/stb) Generic Dynamic Array implementation in C

## Quickstart

```c
#define VEC_IMPLEMENTATION
#include "vec.h"

int main(void) {
  // Initialize vector
  int *int_vec = vec_init(sizeof(int), NULL);

  // pass free function if necessary
  char **string_vec = vec_init(sizeof(char *), free);

  // push multiple elements
  // always reassign vector if a function returns void*
  int new_elements[] = {1, 2, 3};
  int_vec = vec_push(int_vec, new_elements,
                     sizeof(new_elements) / sizeof(new_elements[0]));

  // you can use vec_push_val macro for pushing single element.
  // this macro and vec_insert_val handle reassignment internally so
  // do not use them with manual assignment (e.g., v = vec_push_val(v, x)).
  vec_push_val(int_vec, 4);

  // vectors also implements iterator interface
  vec_foreach(iterator, int_vec) { printf("%d ", *iterator); }

  // Other functions
  // vec_reserve
  // vec_shrink
  // vec_pop
  // vec_get
  // vec_remove_from
  // vec_insert
  // vec_insert_val (macro for single element insertion)
  // vec_clear
  // vec_length
  // vec_capacity
  // vec_size

  // always free vectors after use
  vec_free((void **)&int_vec);
  vec_free((void **)&string_vec);
  return 0;
}
```

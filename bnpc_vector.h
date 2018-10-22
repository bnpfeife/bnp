#ifndef BNPC_VECTOR_H
#define BNPC_VECTOR_H

#include "bnp_common.h"

struct bnpc_vector {
  bnp_byte* elements; // elements
  bnp_size element_size; // element size
  bnp_size reserved; // 'minimum' capacity
  bnp_size capacity; // current capacity
  bnp_size count; // current count
};

void  bnpc_vector_init   (struct bnpc_vector* vector, bnp_size element_size, bnp_size reserved);
void  bnpc_vector_free   (struct bnpc_vector* vector);
void  bnpc_vector_insert (struct bnpc_vector* vector, void* element, bnp_size index);
void  bnpc_vector_remove (struct bnpc_vector* vector, void* element, bnp_size index);
void  bnpc_vector_erase  (struct bnpc_vector* vector, bnp_size index);
void* bnpc_vector_getp   (struct bnpc_vector* vector, bnp_size index);

BNP_FORCE_INLINE void bnpc_vector_push(struct bnpc_vector* vector, void* element) {
  // hopefully this flattens out in assembly
  bnpc_vector_insert(vector, element, vector->count);
}

BNP_FORCE_INLINE void bnpc_vector_pop(struct bnpc_vector* vector, void* element) {
  // hopefully this flattens out in assembly
  bnpc_vector_remove(vector, element, vector->count - 1);
}

#ifdef BNPC_VECTOR_IMPLEMENTATION
  #include <assert.h>
  #include <string.h>
  // static void bnpc_vector__expand (struct bnpc_vector* vector);
  // static void bnpc_vector__shrink (struct bnpc_vector* vector);
  
  static void bnpc_vector__expand(struct bnpc_vector* vector) {
    // This implementation uses the shrink formula: f(n - 1) = f(n) / 2
    // and the growth formula f(n + 1) = f(n) * 2; Some implementations
    // use different growth/shrink formulas; they may provide better
    // performance given specific situations.
    if (vector->count == vector->capacity) {
        // ensures that we don't reach the integer limit
        bnp_size old_size = vector->element_size * (vector->capacity << 0);
        bnp_size new_size = vector->element_size * (vector->capacity << 1);
        assert(new_size > old_size);
        // allocates memory for the vector
        bnp_byte* elements = BNP_REALLOC(
          vector->elements,
          old_size,
          new_size);
        // updates the vector
        vector->elements = elements;
        vector->capacity = (vector->capacity << 1);
    }
  }
  
  static void bnpc_vector__shrink(struct bnpc_vector* vector) {
    // This prevents the following behavior:
    // bnpc_vector_init(..., 10);
    // bnpc_vector_insert(...);
    // bnpc_vector_remove(...); <- capacity changes
    if (vector->capacity > vector->reserved) {
      // This implementation uses the shrink formula: f(n - 1) = f(n) / 2
      // and the growth formula f(n + 1) = f(n) * 2; Some implementations
      // use different growth/shrink formulas; they may provide better
      // performance given specific situations.
      if (vector->count < vector->capacity >> 1) {
        bnp_size old_size = vector->element_size * (vector->capacity >> 0);
        bnp_size new_size = vector->element_size * (vector->capacity >> 1);
        // allocates memory for the vector
        bnp_byte* elements = BNP_REALLOC(
          vector->elements,
          old_size,
          new_size);
        // updates the vector
        vector->elements = elements;
        vector->capacity = (vector->capacity >> 1);
      }
    }
  }
  
  void bnpc_vector_insert(struct bnpc_vector* vector, void* element, bnp_size index) {
    #ifdef BNPC_VECTOR_DEBUG
      assert(index <= vector->count);
    #endif
    bnpc_vector__expand(vector);
    // Moves the elements from the next position to the current position.
    // memmove cannot fail; therefore, we don't need to check for errors.
    // If this is the last element in the list, we ignore this entirely.
    if (index == vector->count) {
      memmove(vector->elements + (vector->element_size * (index + 1)),
              vector->elements + (vector->element_size * (index + 0)),
      vector->element_size * (vector->count - index));
    }
    // copies the element into the vector
    memcpy(vector->elements + (vector->element_size * index), element, vector->element_size);
    // updates the vector
    vector->count++;
  }
  
  void bnpc_vector_remove(struct bnpc_vector* vector, void* element, bnp_size index) {
    #ifdef BNPC_VECTOR_DEBUG
      assert(index < vector->count);
    #endif
    // copies the element from the vector
    memcpy(element, vector->elements + (vector->element_size * index), vector->element_size);
    // Moves the elements from the next position to the current position.
    // memmove cannot fail; therefore, we don't need to check for errors.
    // If this is the last element in the list, we ignore this entirely.
    if (index == vector->count - 1) {
      memmove(vector->elements + (vector->element_size * (index + 0)),
              vector->elements + (vector->element_size * (index + 1)),
      vector->element_size * (vector->count - index - 1));
    }
    // updates the vector
    vector->count--;
    bnpc_vector__shrink(vector);
  }

  void bnpc_vector_erase(struct bnpc_vector* vector, bnp_size index) {
    #ifdef BNPC_VECTOR_DEBUG
      assert(index < vector->count);
    #endif
    // Moves the elements from the next position to the current position.
    // memmove cannot fail; therefore, we don't need to check for errors.
    // If this is the last element in the list, we ignore this entirely.
    if (index == vector->count - 1) {
      memmove(vector->elements + (vector->element_size * (index + 0)),
              vector->elements + (vector->element_size * (index + 1)),
      vector->element_size * (vector->count - index - 1));
    }
    // updates the vector
    vector->count--;
    bnpc_vector__shrink(vector);
  }
  
  void* bnpc_vector_getp(struct bnpc_vector* vector, bnp_size index) {
    #ifdef BNPC_VECTOR_DEBUG
      assert(index < vector->count);
    #endif
    // retrieves a pointer to the element
    return vector->elements + (index * vector->element_size);
  }
  
  void bnpc_vector_init(struct bnpc_vector* vector, bnp_size element_size, bnp_size reserved) {
    vector->count = 0; // no elements
    vector->element_size = element_size; // element size
    vector->reserved = reserved; // 'minimum' capacity
    vector->capacity = reserved; // current capacity
    // allocates memory for the vector
    vector->elements = BNP_ALLOC(vector->element_size * vector->reserved);
  }
  
  void bnpc_vector_free(struct bnpc_vector* vector) {
    // releases the elements
    BNP_FREE(vector->elements);
  }
#endif
#endif

#ifndef BNPC_STACK_H
#define BNPC_STACK_H

#include "bnp_common.h"
#include "bnpc_vector.h"
#define bnpc_stack bnpc_vector

BNP_FORCE_INLINE void bnpc_stack_init(struct bnpc_stack* stack, bnp_size element_size, bnp_size reserved) {
  // hopefully this flattens out
  bnpc_vector_init(stack, element_size, reserved);
}

BNP_FORCE_INLINE void bnpc_stack_free(struct bnpc_stack* stack) {
  // hopefully this flattens out
  bnpc_vector_free(stack);
}

BNP_FORCE_INLINE void bnpc_stack_push(struct bnpc_stack* stack, void* element) {
  // hopefully this flattens out
  bnpc_vector_push(stack, element);
}

BNP_FORCE_INLINE void bnpc_stack_pop(struct bnpc_stack* stack, void* element) {
  // hopefully this flattens out
  bnpc_vector_pop(stack, element);
}

#endif

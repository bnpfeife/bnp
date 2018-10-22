#ifndef BNPC_QUEUE_H
#define BNPC_QUEUE_H

#include "bnp_common.h"
#include "bnpc_vector.h"
#define bnpc_queue bnpc_vector

BNP_FORCE_INLINE void bnpc_queue_init(struct bnpc_queue* queue, bnp_size element_size, bnp_size reserved) {
  // hopefully this flattens out
  bnpc_vector_init(queue, element_size, reserved);
}

BNP_FORCE_INLINE void bnpc_queue_free(struct bnpc_queue* queue) {
  // hopefully this flattens out
  bnpc_vector_free(queue);
}

BNP_FORCE_INLINE void bnpc_queue_enqueue(struct bnpc_queue* queue, void* element) {
  // hopefully this flattens out
  bnpc_vector_push(queue, element);
}

BNP_FORCE_INLINE void bnpc_queue_dequeue(struct bnpc_queue* queue, void* element) {
  // hopefully this flattens out
  bnpc_vector_remove(queue, element, 0); 
}

#endif

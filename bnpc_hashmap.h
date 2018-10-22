#ifndef BNPC_HASHMAP_H
#define BNPC_HASHMAP_H

#include "bnp_common.h"
#include "bnpc_vector.h"
#include "bnpc_list.h"

struct bnpc_hashmap {
  struct bnpc_vector buckets;
  bnp_size k_size; // key size
  bnp_size v_size; // value size
  bnp_size element_count; // element count
  bnp_size reserved; // reserved space
  bnp_size  (*func_hash)(void* key); // hashing function
  bnp_int32 (*func_comp)(void* key_a, void* key_b); // comparison function
};

void              bnpc_hashmap_init         (struct bnpc_hashmap* hashmap, bnp_size k_size, bnp_size v_size, bnp_size reserved, bnp_size (*func_hash)(void* key), bnp_int32 (*func_comp)(void* key_a, void* key_b));
void              bnpc_hashmap_free         (struct bnpc_hashmap* hashmap);
void*             bnpc_hashmap_getp         (struct bnpc_hashmap* hashmap, void* key);
void              bnpc_hashmap__insert      (struct bnpc_hashmap* hashmap, void* key, void* value);
bnp_int32         bnpc_hashmap__remove      (struct bnpc_hashmap* hashmap, void* key, void* value);
bnp_int32         bnpc_hashmap__erase       (struct bnpc_hashmap* hashmap, void* key);
void              bnpc_hashmap__resize      (struct bnpc_hashmap* hashmap);
struct bnpc_list* bnpc_hashmap__getBucket   (struct bnpc_hashmap* hashmap, void* key);
void              bnpc_hashmap__initBuckets (struct bnpc_vector* buckets, bnp_size size, bnp_size capacity);
void              bnpc_hashmap__freeBuckets (struct bnpc_vector* buckets);

BNP_FORCE_INLINE void bnpc_hashmap_insert(struct bnpc_hashmap* hashmap, void* key, void* value) {
  // bnpc_hashmap_* are the 'public' facing functions. These functions may
  // resize the container. bnpc_hashmap__resize relies on bnpc_hashmap__*
  // functions; therefore none of the bnpc_hashmap__* functions can call
  // bnpc_hashmap__resize.
  bnpc_hashmap__resize(hashmap);
  bnpc_hashmap__insert(hashmap, key, value);
}

BNP_FORCE_INLINE bnp_int32 bnpc_hashmap_remove(struct bnpc_hashmap* hashmap, void* key, void* value) {
  // bnpc_hashmap_* are the 'public' facing functions. These functions may
  // resize the container. bnpc_hashmap__resize relies on bnpc_hashmap__*
  // functions; therefore none of the bnpc_hashmap__* functions can call
  // bnpc_hashmap__resize.
         bnpc_hashmap__resize(hashmap);
  return bnpc_hashmap__remove(hashmap, key, value);
}

BNP_FORCE_INLINE bnp_int32 bnpc_hashmap_erase(struct bnpc_hashmap* hashmap, void* key) {
  // bnpc_hashmap_* are the 'public' facing functions. These functions may
  // resize the container. bnpc_hashmap__resize relies on bnpc_hashmap__*
  // functions; therefore none of the bnpc_hashmap__* functions can call
  // bnpc_hashmap__resize.
         bnpc_hashmap__resize(hashmap);
  return bnpc_hashmap__erase(hashmap, key);
}

#ifdef BNPC_HASHMAP_IMPLEMENTATION
  #define BNPC_HASHMAP_LOADMAX 0.7
  #define BNPC_HASHMAP_LOADMIN 0.3
  #define BNPC_HASHMAP_KEY_OFFSET(H) 0
  #define BNPC_HASHMAP_VAL_OFFSET(H) H->k_size
  #define BNPC_HASHMAP_ELEMENT_SIZE(H) H->k_size + H->v_size

  void bnpc_hashmap_init(
    struct bnpc_hashmap* hashmap,
    bnp_size k_size,
    bnp_size v_size,
    bnp_size reserved,
    bnp_size  (*func_hash)(void* key),
    bnp_int32 (*func_comp)(void* key_a, void* key_b)) {
    hashmap->element_count = 0; // no elements
    hashmap->k_size = k_size; // key size
    hashmap->v_size = v_size; // value size
    hashmap->reserved = reserved; // reserved buckets
    hashmap->func_hash = func_hash; // hashing function
    hashmap->func_comp = func_comp; // comparison function
    bnp_size size = BNPC_HASHMAP_ELEMENT_SIZE(hashmap);
    // initializes the buckets
    bnpc_hashmap__initBuckets(&hashmap->buckets, size, hashmap->reserved);    
  }

  void bnpc_hashmap_free(struct bnpc_hashmap* hashmap) {
    // releases the buckets
    bnpc_hashmap__freeBuckets(&hashmap->buckets);
  }

  void* bnpc_hashmap_getp(struct bnpc_hashmap* hashmap, void* key) {
    struct bnpc_list* bucket = bnpc_hashmap__getBucket(hashmap, key);
    struct bnpc_node* beg = bucket->beg;
    struct bnpc_node* end = bucket->end;
    for(struct bnpc_node* node = beg->next; node != end; node = node->next) {
      if (!hashmap->func_comp(node->elem, key)) {
        return node->elem + hashmap->k_size;
      }
    }
    return NULL;
  }
  
  bnp_int32 bnpc_hashmap_contains(struct bnpc_hashmap* hashmap, void* key) {
    struct bnpc_list* bucket = bnpc_hashmap__getBucket(hashmap, key);
    struct bnpc_node* beg = bucket->beg;
    struct bnpc_node* end = bucket->end;
    for(struct bnpc_node* node = beg->next; node != end; node = node->next) {
      if (!hashmap->func_comp(node->elem, key)) {
        return 1;
      }
    }
    return 0;
  }
  
  bnp_int32 bnpc_hashmap__remove(struct bnpc_hashmap* hashmap, void* key, void* value) {
    struct bnpc_list* bucket = bnpc_hashmap__getBucket(hashmap, key);
    struct bnpc_node* beg = bucket->beg;
    struct bnpc_node* end = bucket->end;
    for(struct bnpc_node* node = beg->next; node != end; node = node->next) {
      if (!hashmap->func_comp(node->elem, key)) {
        memcpy(value, node->elem + hashmap->k_size, hashmap->v_size);
        bnpc_list_erase(bucket, node);
        hashmap->element_count--;
        return 1;
      }
    }
    return 0;
  }

  bnp_int32 bnpc_hashmap__erase(struct bnpc_hashmap* hashmap, void* key) {
    struct bnpc_list* bucket = bnpc_hashmap__getBucket(hashmap, key);
    struct bnpc_node* beg = bucket->beg;
    struct bnpc_node* end = bucket->end;
    for(struct bnpc_node* node = beg->next; node != end; node = node->next) {
      if (!hashmap->func_comp(node->elem, key)) {
        bnpc_list_erase(bucket, node);
        hashmap->element_count--;
        return 1;
      }
    }
    return 0;
  }

  struct bnpc_list* bnpc_hashmap__getBucket(struct bnpc_hashmap* hashmap, void* key) {
    // Ensures that the bucket selected by hashmap->func_hash(key) is
    // within the limits of the actual vector.
    bnp_size index = hashmap->func_hash(key) % hashmap->buckets.count;
    return bnpc_vector_getp(&hashmap->buckets, index);
  }

  void bnpc_hashmap__initBuckets(struct bnpc_vector* buckets, bnp_size size, bnp_size capacity) {
    // The hashmap contains buckets and those buckets contain elements.
    // Instead of linked-lists, vectors are used to improve cache
    // efficiency. bnpc_vector* is used for the vector implementation.      
    bnpc_vector_init(buckets, sizeof(struct bnpc_list), capacity);
    for (bnp_size i = 0; i < capacity; i++) {
    // initializes the bucket
      struct bnpc_list bucket;
      bnpc_list_init(&bucket, size);
      bnpc_vector_push(buckets, &bucket);
    }
  }

  void bnpc_hashmap__freeBuckets(struct bnpc_vector* buckets) {
    // The hashmap contains buckets and those buckets contain elements.
    // Instead of linked-lists, vectors are used to improve cache
    // efficiency. bnpc_vector* is used for the vector implementation.      
    for (bnp_size i = 0; i < buckets->count; i++) {
      // releases the buckets	
      bnpc_list_free(bnpc_vector_getp(buckets, i));
    }
    bnpc_vector_free(buckets);
  }
  
  void bnpc_hashmap__insert(struct bnpc_hashmap* hashmap, void* key, void* value) {
    // Keys and values are sequentially packed into a vector. This approach
    // attempts to keep cache-misses to a minimum by avoiding linked-lists;
    // however, a buffer is needed to transfer the keys and values into
    // the hashmap.
    bnp_byte* buffer = BNP_ALLOC(BNPC_HASHMAP_ELEMENT_SIZE(hashmap));
    memcpy(buffer + BNPC_HASHMAP_KEY_OFFSET(hashmap), key  , hashmap->k_size);
    memcpy(buffer + BNPC_HASHMAP_VAL_OFFSET(hashmap), value, hashmap->v_size);

    struct bnpc_list* bucket = bnpc_hashmap__getBucket(hashmap, key);
    struct bnpc_node* beg = bucket->beg;
    struct bnpc_node* end = bucket->end;
    // iterates through the elements
    for(struct bnpc_node* node = beg->next; node != end; node = node->next) {
      if (!hashmap->func_comp(node->elem, key)) {
        // An element with the same key has been located. In these cases,
        // the element is updated with the new value; this should be the
        // same as with C++'s STL. Since the buffer is already built, the
        // buffer is copied over the element.
        memcpy(node->elem, buffer, BNPC_HASHMAP_ELEMENT_SIZE(hashmap));
        // releases the buffer
        free(buffer);
        return;
      }
    }
    // inserts the element
    bnpc_list_insert(bucket, buffer);
    hashmap->element_count++;
    // releases the buffer
    free(buffer);
  }

  void bnpc_hashmap__resize(struct bnpc_hashmap* hashmap) {
    // In the best-case scenario, each element has a unique bucket;
    // therefore, we calculate the load-factor by comparing the amount of
    // buckets and the amount of elements. There isn't a defined optimal
    // time to increase/decrease the quantity of bucket.
    double load_factor =
        (double)hashmap->element_count /
        (double)hashmap->buckets.count;
    bnp_int32 increase = (load_factor >= BNPC_HASHMAP_LOADMAX);
    bnp_int32 decrease = (load_factor <= BNPC_HASHMAP_LOADMIN) && (hashmap->buckets.count > hashmap->reserved);
      
    if (increase || decrease) {
      struct bnpc_vector buckets;
      // temporarily copy the elements outside the hashmap
      memcpy(&buckets, &hashmap->buckets, sizeof buckets);
      // Since the elements are temporarily stored outside of the hashmap,
      // hashmap's element_count is incorrect; therefore, we reset it to
      // zero. Once all elements have been inserted, element_count will
      // reflect the accurate count.
      hashmap->element_count = 0;
      
      bnp_size size = BNPC_HASHMAP_ELEMENT_SIZE(hashmap);
          bnp_size capacity = decrease
        ? hashmap->buckets.capacity >> 1  // decreases the capacity
        : hashmap->buckets.capacity << 1; // increases the capacity
      bnpc_hashmap__initBuckets(&hashmap->buckets, size, capacity);

      for (bnp_size i = 0; i < buckets.count; i++) {
        struct bnpc_list* bucket = bnpc_vector_getp(&buckets, i);
        struct bnpc_node* beg = bucket->beg;
        struct bnpc_node* end = bucket->end;
        for(struct bnpc_node* node = beg->next; node != end; node = node->next) {
          // Retrieves a pointer to the element; since keys and values are
          // packed contiguously, the keys and values can be found by
          // offseting the element pointer.
          void* key   = node->elem + BNPC_HASHMAP_KEY_OFFSET(hashmap);
          void* value = node->elem + BNPC_HASHMAP_VAL_OFFSET(hashmap);
          bnpc_hashmap__insert(hashmap, key, value);
        }
      }
      // releases the buckets
      bnpc_hashmap__freeBuckets(&buckets);
    }
  }
#endif
#endif

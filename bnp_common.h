#ifndef BNP_COMMON_H
#define BNP_COMMON_H

#ifdef _MSC_VER
  // signed
  typedef __int8 bnp_int8;
  typedef __int16 bnp_int16;
  typedef __int32 bnp_int32;
  typedef __int64 bnp_int64;
  // unsigned
  typedef unsigned __int8 bnp_uint8;
  typedef unsigned __int16 bnp_uint16;
  typedef unsigned __int32 bnp_uint32;
  typedef unsigned __int64 bnp_uint64;
  // special
  typedef bnp_uint8 bnp_byte;
#else
  #include <stdint.h>
  // signed
  typedef int8_t bnp_int8;
  typedef int16_t bnp_int16;
  typedef int32_t bnp_int32;
  typedef int64_t bnp_int64;
  // unsigned
  typedef uint8_t bnp_uint8;
  typedef uint16_t bnp_uint16;
  typedef uint32_t bnp_uint32;
  typedef uint64_t bnp_uint64;
  // special
  typedef bnp_uint8 bnp_byte;
#endif

#if _WIN32 || _WIN64
  #ifdef _WIN64
    typedef bnp_uint64 bnp_size;
  #else
    typedef bnp_uint32 bnp_size;
  #endif
#else
  #ifdef __x86_64
    typedef bnp_uint64 bnp_size;
  #else
    typedef bnp_uint32 bnp_size;
  #endif
#endif

// includes function implementations
#ifdef BNP_IMPLEMENTATION
  #define BNP_COLLECTION_IMPLEMENTATION
  #define BNP_SPATIAL_IMPLEMENTATION
#endif

// includes function debugging (assert)
#ifdef BNP_DEBUG
  #define BNP_COLLECTION_DEBUG
  #define BNP_SPATIAL_DEBUG
#endif

// enables implementations of collections
#ifdef BNP_COLLECTION_IMPLEMENTATION
  #define BNPC_HASHMAP_IMPLEMENTATION // completed
  #define BNPC_LIST_IMPLEMENTATION    // not-started
  #define BNPC_QUEUE_IMPLEMENTATION   // unneeded
  #define BNPC_STACK_IMPLEMENTATION   // unneeded
  #define BNPC_VECTOR_IMPLEMENTATION  // completed
#endif

// enables debugging for collections
#ifdef BNP_COLLECTION_DEBUG
  #define BNPC_HASHMAP_DEBUG // unneeded
  #define BNPC_LIST_DEBUG    // not-implemented
  #define BNPC_QUEUE_DEBUG   // unneeded
  #define BNPC_STACK_DEBUG   // unneeded
  #define BNPC_VECTOR_DEBUG  // completed
#endif

// force-inline
#define BNP_FORCE_INLINE __attribute__ ((always_inline)) inline

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
BNP_FORCE_INLINE void* bnp_alloc(const bnp_size size) {
  void* pointer = malloc(size);
  assert(pointer);
  return pointer;
}

BNP_FORCE_INLINE void* bnp_realloc(
  void* src,
  const bnp_size old_size,
  const bnp_size new_size) {
  void* dst = malloc(new_size);
  assert(dst);
  memcpy(dst, src, (old_size < new_size) ? old_size : new_size);
  free(src);
  return dst;
}

#if !defined(BNP_ALLOC)   ||\
    !defined(BNP_REALLOC) ||\
    !defined(BNP_FREE)
  #define BNP_ALLOC(S)       bnp_alloc(S)
  #define BNP_REALLOC(P,O,N) bnp_realloc(P,O,N)
  #define BNP_FREE(P)        free(P)
#endif
#endif

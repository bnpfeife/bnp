#ifndef BNPC_LIST_H
#define BNPC_LIST_H

#include "bnp_common.h"

struct bnpc_node {
  struct bnpc_node* next;
  struct bnpc_node* prev;
  bnp_byte* elem;
};

struct bnpc_list {
  struct bnpc_node* beg;
  struct bnpc_node* end;
  bnp_size elem_size;
  bnp_size count;
};

struct bnpc_node* bnpc_node_init   (struct bnpc_node*, struct bnpc_node*, bnp_byte*);
void              bnpc_node_free   (struct bnpc_node*);
void              bnpc_list_init   (struct bnpc_list*, const bnp_size);
void              bnpc_list_free   (struct bnpc_list*);
bnp_int32         bnpc_list_empty  (struct bnpc_list*);
void              bnpc_list_insert (struct bnpc_list*, void*);
void              bnpc_list_remove (struct bnpc_list*, struct bnpc_node*, void*);
void              bnpc_list_erase  (struct bnpc_list*, struct bnpc_node*);
struct bnpc_node* bnpc_list_getp   (struct bnpc_list*, bnp_size);
struct bnpc_node* bnpc_list_beg    (struct bnpc_list*);
struct bnpc_node* bnpc_list_end    (struct bnpc_list*);

#ifdef BNPC_LIST_IMPLEMENTATION

  void bnpc_list_init(struct bnpc_list* list, const bnp_size elem_size) {
    list->beg = bnpc_node_init(NULL, NULL, NULL);
    list->end = bnpc_node_init(NULL, NULL, NULL);
    list->beg->next = list->end;
    list->end->prev = list->beg;
    list->elem_size = elem_size;
    list->count = 0;
  }

  void bnpc_list_free(struct bnpc_list* list) {
    while (!bnpc_list_empty(list)) {
      bnpc_node_free(list->beg->next);
    }
    bnpc_node_free(list->beg);
    bnpc_node_free(list->end);
  }

  bnp_int32 bnpc_list_empty(struct bnpc_list* list) {
    return (list->beg->next == list->end) &&
           (list->end->prev == list->beg);
  }

  void bnpc_list_insert(struct bnpc_list* list, void* elem) {
    struct bnpc_node* node = bnpc_node_init(list->beg->next, list->beg, NULL);
    node->elem = BNP_ALLOC(list->elem_size);
    memcpy(node->elem, elem, list->elem_size);
    list->count++;
  }

  void bnpc_list_remove(struct bnpc_list* list, struct bnpc_node* node, void* elem) {
    memcpy(elem, node->elem, list->elem_size);
    bnpc_node_free(node);
    list->count--;
  }

  void bnpc_list_erase(struct bnpc_list* list, struct bnpc_node* node) {
    bnpc_node_free(node);
    list->count--;
  }

  struct bnpc_node* bnpc_list_getp(struct bnpc_list* list, bnp_uint64 elem) {
    for(struct bnpc_node* itr = list->beg->next; itr != list->end; itr = itr->next) {
      #ifdef BNPC_LIST_DEBUG
        assert(elem > 0);
      #endif
      if (elem-- == 0) {
        return itr;
      }
    }
    return NULL;
  }

  struct bnpc_node* bnpc_list_end(struct bnpc_list* list) {
    assert(!bnpc_list_empty(list));
    return list->end->prev;
  }

  struct bnpc_node* bnpc_list_beg(struct bnpc_list* list) {
    assert(!bnpc_list_empty(list));
    return list->beg->next;
  }

  struct bnpc_node* bnpc_node_init(
    struct bnpc_node* next,
    struct bnpc_node* prev,
    bnp_byte* elem) {
    struct bnpc_node* node = BNP_ALLOC(sizeof * node);
    node->elem = elem;
    node->next = next;
    node->prev = prev;
    if (node->next) node->next->prev = node;
    if (node->prev) node->prev->next = node;
    return node;
  }

  void bnpc_node_free(struct bnpc_node* node) {
    #ifdef bnpc_list_DEBUG
      assert(node);
    #endif
    if (node->next) node->next->prev = node->prev;
    if (node->prev) node->prev->next = node->next;
    BNP_FREE(node->elem);
    BNP_FREE(node);
  }

#endif
#endif
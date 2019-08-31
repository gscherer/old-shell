#ifndef GES_COLLECTION_ARRAY_H
#define GES_COLLECTION_ARRAY_H

#include "../interfaces/collection.h"

#define ARRAY_INIT_ALLOC_ITEMS 1000
#define ARRAY_REALLOC_ITEMS 1000

typedef struct type_Array *Array;
typedef struct type_Array_Iterator *Array_Iterator;

struct type_Array {
	size_t item_count;
	size_t allocated;
	void **items;
};

struct type_Array_Iterator {
    size_t next_idx;
    size_t curr_idx;
};

Collection_Interface
collection_interface_array();

Array
collection_array();

#endif

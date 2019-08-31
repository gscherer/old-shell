#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "array.h"

static void 
array_add(Collection collection, Item item)
{
	Array array;

	assert(collection != NULL && item != NULL);

	array = collection->this;

	if (array->item_count == array->allocated) {
		array->items = realloc(array->items, 
			sizeof(void *) * (array->allocated + ARRAY_REALLOC_ITEMS));
		array->allocated += ARRAY_REALLOC_ITEMS;
	}

	array->items[array->item_count++] = item;

	assert(collection_find(collection, collection_item_key(collection, item)) != NULL);
}

static void
array_remove(Collection collection, Item item)
{
	Array array;
	size_t i;

	assert(collection != NULL && item != NULL);

	array = collection->this;

	for (i = 0; i < array->item_count; i++) {
		if (item == array->items[i]) {
			collection_item_deconstruct(collection, item);
			
			while (i < (array->item_count - 1)) {
				array->items[i] = array->items[i + 1];
				i++;
			}
			
			array->item_count--;			
			break;
		}
	}

    assert(collection_find(collection, collection_item_key(collection, item)) == NULL);
}

static Item
array_find(Collection collection, void *key)
{
	Array array;
	size_t i;

	assert(collection != NULL);
	assert(key != NULL);

	array = collection->this;

	for (i = 0; i < array->item_count; i++) {
		if (collection_item_compare(
				collection, 
				key, 
				collection_item_key(collection, array->items[i])) == 0) {
			return array->items[i];
		}
	}

	return NULL;
}

static size_t
array_count(Collection collection)
{
	return ((Array)(collection->this))->item_count;
}

static int
array_is_empty(Collection collection)
{
	Array array;

	array = collection->this;

	return (array->item_count) ? 0 : 1;
}

static void
array_remove_all(Collection collection)
{
	Array array;
	size_t i;

	assert(collection != NULL);

	array = collection->this;

	for (i = 0; i < array->item_count; i++) {
		collection_item_deconstruct(collection, array->items[i]);
	}

	array->item_count = 0;
}

static void 
array_deconstruct(Collection collection)
{
	Array array;
	size_t i;

	assert(collection != NULL);

	array = collection->this;

	for (i = 0; i < array->item_count; i++) {
		collection_item_deconstruct(collection, array->items[i]);
	}

	free(array->items);
	free(array);
}

static int
iter_has_next(Iterator iterator)
{
    Array_Iterator array_iter;
    Array array;

    assert(iterator != NULL);

    array = iterator->collection->this;
    array_iter = iterator->this;

    return (array_iter->next_idx < array->item_count) ? 1 : 0;
}

static Item
iter_next(Iterator iterator)
{
    Array_Iterator array_iter;
    Array array;

    assert(iterator != NULL);

    array_iter = iterator->this;
    array = iterator->collection->this;

    if (array_iter->next_idx >= array->item_count) {
        return NULL;
    }
    
    array_iter->curr_idx = array_iter->next_idx;
    
    return array->items[array_iter->next_idx++];
}

static void
iter_remove(Iterator iterator)
{
    Array_Iterator array_iter;
    Array array;

    assert(iterator != NULL);

    array_iter = iterator->this;
    array = iterator->collection->this;

    if (array->item_count <= 0 || array_iter->curr_idx == -1) {
        return;
    }

    collection_item_deconstruct(iterator->collection, array->items[array_iter->curr_idx]);

    size_t i;
    i = array_iter->curr_idx;

    while (i < (array->item_count - 1)) {
        array->items[i] = array->items[i + 1];
        i++;
    }

    array_iter->curr_idx = -1;
    array_iter->next_idx--;
    array->item_count--;
}

static void 
iter_reset(Iterator iterator)
{
    Array_Iterator array_iter;
    Array array;

    assert(iterator != NULL);

    array_iter = iterator->this;
    array = iterator->collection->this;

    array_iter->curr_idx = -1;
    array_iter->next_idx = 0;
}

static void
iter_deconstruct(Iterator iterator)
{
    assert(iterator != NULL);
    
    free(iterator->this);
}

Iterator
array_iterator(Collection collection)
{
    Iterator_Interface interface;
    Array_Iterator this;
    Array array;
    
    array = collection->this;

    this = (Array_Iterator)calloc(1, sizeof(struct type_Array_Iterator));
    this->next_idx = 0;
    this->curr_idx = -1;

	interface = iterator_interface(iter_has_next, iter_next, iter_remove, 
		iter_reset, iter_deconstruct);

    return iterator_construct(this, collection, interface);
}

Array
collection_array()
{
	Array array;

	array = (Array)calloc(1, sizeof(struct type_Array));
	array->items = (void **)calloc(ARRAY_INIT_ALLOC_ITEMS, sizeof(void *));
	array->item_count = 0;
	array->allocated = ARRAY_INIT_ALLOC_ITEMS;

	return array;
}

Collection_Interface
collection_interface_array()
{
	return collection_interface(
		array_add, 
		array_remove, 
		array_find, 
		array_count, 
		array_is_empty, 
		array_remove_all, 
		array_iterator, 
		array_deconstruct);
}

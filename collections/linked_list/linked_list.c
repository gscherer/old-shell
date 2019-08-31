#include <stdlib.h>
#include <assert.h>

#include "linked_list.h"

static Linked_List_Node
linked_list_node(Item item, Linked_List_Node next)
{
	Linked_List_Node node;

	node = (Linked_List_Node)calloc(1, sizeof(struct type_Linked_List_Node));
	node->item = item;
	node->next = next;

	return node;
}

static void 
linked_list_add(Collection collection, Item item)
{
	Linked_List_Node node;
	Linked_List ll;

	assert(collection != NULL && item != NULL);

	ll = collection->this;
	node = linked_list_node(item, ll->head);
	ll->head = node;
	ll->item_count++;
}

static void
linked_list_remove(Collection collection, Item item)
{
	Linked_List_Node node, prev_node;
	Linked_List ll;

	assert(collection != NULL && item != NULL);

	ll = collection->this;
	prev_node = node = ll->head;

	while (node != NULL && node->item != item) {
		prev_node = node;
		node = node->next;
	}

	if (node != NULL) {
		if (node == ll->head) {
			ll->head = node->next;
		} else {
			prev_node->next = node->next;
		}

		ll->item_count--;
		collection_item_deconstruct(collection, node->item);
		free(node);
	}	
}

static Item
linked_list_find(Collection collection, void *key)
{
	Linked_List_Node node;
	Linked_List ll;
	void *curr_key;

	assert(collection != NULL && key != NULL);

	ll = collection->this;

	for (node = ll->head; node != NULL; node = node->next) {
		if (collection_item_compare(collection, key, 
				collection_item_key(collection, node->item)) == 0) 
		{
			return node->item;
		}
	}

	return NULL;
}

static size_t
linked_list_count(Collection collection)
{
    assert(collection != NULL);

	return ((Linked_List)(collection->this))->item_count;
}

static int
linked_list_is_empty(Collection collection)
{
    assert(collection != NULL);

	return linked_list_count(collection) ? 0 : 1;
}

static void
linked_list_remove_all(Collection collection)
{
	Linked_List ll;
	Linked_List_Node node, *node_save;
	Item item;

    assert(collection != NULL);

	ll = collection->this;
	node = ll->head;

	while (node != NULL) {
		node_save = &node;
		node = node->next;
		collection_item_deconstruct(collection, (*node_save)->item);
		free(*node_save);
		ll->item_count--;
	}
}

static void 
linked_list_deconstruct(Collection collection)
{
	Linked_List_Node node, prev_node;
	Linked_List ll;

	assert(collection != NULL);

	ll = collection->this;
	prev_node = NULL;
	node = ll->head;

	while (node != NULL) {
		collection_item_deconstruct(collection, node->item);
		prev_node = node;
		node = node->next;
		free(prev_node);
	}

	free(collection->this);
}

static int
iter_has_next(Iterator iterator)
{
	Linked_List_Iterator ll_iter;

    assert(iterator != NULL);

	ll_iter = iterator->this;

	return (ll_iter->next_node == NULL) ? 0 : 1;
}

static Item
iter_next(Iterator iterator)
{
	Linked_List_Iterator ll_iter;
	Linked_List_Node node;

    assert(iterator != NULL);

	ll_iter = iterator->this;
	node = ll_iter->next_node;

	if (node == NULL) {
		return NULL;
	}

	ll_iter->next_node = node->next;
	ll_iter->curr_node = node;

	return node->item;

}

static void
iter_remove(Iterator iterator)
{
	Linked_List_Iterator ll_iter;
	Collection collection;

    assert(iterator != NULL);
	
    ll_iter = iterator->this;
	collection = iterator->collection;

	if (ll_iter->curr_node != NULL) {
		collection_remove(collection, ll_iter->curr_node->item);
		ll_iter->curr_node = NULL;
	}
}

static void
iter_reset(Iterator iterator)
{
	Linked_List ll;
	Linked_List_Iterator ll_iter;

    assert(iterator != NULL);
	
    ll = iterator->collection->this;
	ll_iter = iterator->this;

	ll_iter->next_node = ll->head;
	ll_iter->curr_node = NULL;
}

static void
iter_deconstruct(Iterator iterator)
{
    assert(iterator != NULL);
    
    free(iterator->this);
}

static Iterator
linked_list_iterator(Collection collection)
{
	Iterator_Interface interface;
	Linked_List_Iterator this;
	Linked_List ll;

    assert(collection != NULL);

	ll = collection->this;

	this = (Linked_List_Iterator)calloc(1, sizeof(struct type_Linked_List_Iterator));
	this->next_node = ll->head;
	this->curr_node = NULL;

	interface = iterator_interface(iter_has_next, iter_next, iter_remove, 
		iter_reset, iter_deconstruct);

	return iterator_construct(this, collection, interface);
}

Collection_Interface
collection_interface_linked_list()
{
	return collection_interface(
			linked_list_add, 
			linked_list_remove,
			linked_list_find, 
			linked_list_count, 
			linked_list_is_empty, 
			linked_list_remove_all, 
			linked_list_iterator,
			linked_list_deconstruct);
}

Linked_List
collection_linked_list()
{
	Linked_List ll;

	ll = (Linked_List)calloc(1, sizeof(struct type_Linked_List));
	ll->head = (Linked_List_Node)NULL;
	ll->item_count = 0;

	return ll;
}

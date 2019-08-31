#include <stdlib.h>
#include <assert.h>

#include "collection.h"

/**
 *
 */
Collection
collection_construct_(void *this, Item_Interface item_interface, 
	Collection_Interface interface)
{
	Collection collection;
	collection = (Collection)calloc(1, sizeof(struct type_Collection));
	collection->this = this;
	collection->item_interface = item_interface;
	collection->interface = interface;
	return collection;
}

/**
 *
 */
void
collection_deconstruct(Collection collection)
{
	collection->interface->deconstruct(collection);
	free(collection->item_interface);
	free(collection->interface);
	free(collection);
}

/**
 *
 */
Collection_Interface
collection_interface(
	void (*add)(Collection collection, Item item),
	void (*remove_)(Collection collection, Item item),
	Item (*find)(Collection collection, void *key),
	size_t (*count)(Collection collection),
	int (*isEmpty)(Collection collection),
	void (*removeAll)(Collection collection),
	Iterator (*iterator)(Collection collection),
	void (*deconstruct)(Collection collection)) 
{
	Collection_Interface interface;
	interface = (Collection_Interface)calloc(1, sizeof(struct interface_Collection));
	interface->add = add;
	interface->remove = remove_;
	interface->find = find;
	interface->count = count;
	interface->isEmpty = isEmpty;
	interface->removeAll = removeAll;
	interface->iterator = iterator;
	interface->deconstruct = deconstruct;
	return interface;
}

/**
 *
 */
int
collection_has(Collection collection, Item item)
{
	Item found;
	assert(collection != NULL && item != NULL);
	found = collection_find(collection, collection_item_key(collection, item));
	return (found == NULL) ? 0 : 1;
}

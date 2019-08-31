#ifndef GES_COLLECTION_H
#define GES_COLLECTION_H

#include "typedefs.h"
#include "item.h"
#include "iterator.h"

#define collection_construct(name, item_name) \
	collection_construct_(collection_##name(), item_interface_##item_name(), \
		collection_interface_##name())

#define collection_add(collection, item) \
	(collection)->interface->add((collection), (item))

#define collection_remove(collection, item) \
	(collection)->interface->remove((collection), (item))

#define collection_find(collection, key) \
	(collection)->interface->find((collection), (key))

#define collection_count(collection) \
	(collection)->interface->count((collection))

#define collection_is_empty(collection) \
	(collection)->interface->isEmpty((collection))

#define collection_remove_all(collection) \
	(collection)->interface->removeAll((collection))

#define collection_iterator(collection) \
	(collection)->interface->iterator((collection))

#define collection_item_key(collection, item) \
	(collection)->item_interface->key((item))

#define collection_item_compare(collection, key, other_key) \
	(collection)->item_interface->compare((key), (other_key))

#define collection_item_to_string(collection, item) \
	(collection)->item_interface->toString((item))

#define collection_item_deconstruct(collection, item) \
	(collection)->item_interface->deconstruct((item))


struct type_Collection {
	void *this;
	Collection_Interface interface;
	Item_Interface item_interface;
};

struct interface_Collection {
	void (*add)(Collection /* collection */, Item /* item */);
	void (*remove)(Collection /* collection */, Item /* item */);
	Item (*find)(Collection /* collection */, void * /* key */);
	size_t (*count)(Collection /* collection */);
	int (*isEmpty)(Collection /* collection */);
	void (*removeAll)(Collection /* collection */);
	Iterator (*iterator)(Collection /* collection */);
	void (*deconstruct)(Collection /* collection */);
};

Collection
collection_construct_(void * /* this */, Item_Interface /* item_interface */, 
	Collection_Interface /* interface */);

void
collection_deconstruct(Collection /* collection */);

Collection_Interface
collection_interface(
	void (*add)(Collection /* collection */, Item /* item */),
	void (*remove)(Collection /* collection */, Item /* item */),
	Item (*find)(Collection /* collection */, void * /* key */),
	size_t (*count)(Collection /* collection */),
	int (*isEmpty)(Collection /* collection */),
	void (*removeAll)(Collection /* collection */),
	Iterator (*iterator)(Collection /* collection */),
	void (*deconstruct)(Collection /* collection */)
);

int
collection_has(Collection /* collection */, Item /* item */);

#endif

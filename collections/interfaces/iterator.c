#include <stdlib.h>
#include <assert.h>

#include "iterator.h"

Iterator_Interface
iterator_interface(
	int (*hasNext)(Iterator iterator),
	Item (*next)(Iterator iterator),
	void (*remove_)(Iterator iterator),
	void (*reset)(Iterator iterator),
	void (*deconstruct)(Iterator iterator))
{
	Iterator_Interface interface;
	interface = (Iterator_Interface)calloc(1, sizeof(struct interface_Iterator));
	interface->hasNext = hasNext;
	interface->next = next;
	interface->remove = remove_;
	interface->reset = reset;
	interface->deconstruct = deconstruct;
	return interface;
}

Iterator
iterator_construct(void *this, Collection collection, Iterator_Interface interface)
{
	Iterator iterator;
	iterator = (Iterator)calloc(1, sizeof(struct type_Iterator));
	iterator->this = this;
	iterator->collection = collection;
	iterator->interface = interface;
	return iterator;
}

void
iterator_deconstruct(Iterator iterator)
{
	iterator->interface->deconstruct(iterator);
	free(iterator->interface);
	free(iterator);
}
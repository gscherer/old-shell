#ifndef GES_ITERATOR_H
#define GES_ITERATOR_H

#include "typedefs.h"
#include "collection.h"

#define iterator_has_next(iterator) (iterator)->interface->hasNext((iterator))
#define iterator_next(iterator) (iterator)->interface->next((iterator))
#define iterator_remove(iterator) (iterator)->interface->remove((iterator))
#define iterator_reset(iterator) (iterator)->interface->reset((iterator))

struct interface_Iterator {
	int (*hasNext)(Iterator /* iterator */);
	Item (*next)(Iterator /* iterator */);
	void (*remove)(Iterator /* iterator */);
	void (*reset)(Iterator /* iterator */);
	void (*deconstruct)(Iterator /* iterator */);
};

struct type_Iterator {
	void *this;
	Collection collection;
	Iterator_Interface interface;
};

Iterator_Interface
iterator_interface(
	int (*hasNext)(Iterator /* iterator */),
	Item (*next)(Iterator /* iterator */),
	void (*remove_)(Iterator /* iterator */),
	void (*reset)(Iterator /* iterator */),
	void (*deconstruct)(Iterator /* iterator */));

Iterator
iterator_construct(void * /* this */, Collection /* collection */, 
	Iterator_Interface /* interface */);

void
iterator_deconstruct(Iterator /* iterator */);

#endif

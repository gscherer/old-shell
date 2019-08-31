#ifndef GES_ITEM_H
#define GES_ITEM_H

#include "typedefs.h"

struct interface_Item {
	void *(*key)(Item /* item */);
	int (*compare)(void * /* key */, void * /* other_key */);
	char *(*toString)(Item /* item */);
	void (*deconstruct)(Item /* item */);
};

Item_Interface
item_interface(
	void *(*key)(Item /* item */), 
	int (*compare)(void * /* key */, void * /* other_key */), 
	char *(*toString)(Item /* item */), 
	void (*deconstruct)(Item /* item */)
);

#endif

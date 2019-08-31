#include <stdlib.h>

#include "item.h"

Item_Interface
item_interface(
	void *(*key)(Item /* item */),
	int (*compare)(void * /* key */, void * /* other_key */),
	char *(*toString)(Item /* item */),
	void (*deconstruct)(Item /* item */)) 
{
	Item_Interface interface;
	interface = (Item_Interface)calloc(1, sizeof(struct interface_Item));
	interface->key = key;
	interface->compare = compare;
	interface->toString = toString;
	interface->deconstruct = deconstruct;
	return interface;
}
#ifndef GES_COLLECTION_LINKED_LIST_H
#define GES_COLLECTION_LINKED_LIST_H

#include "../interfaces/collection.h"

typedef struct type_Linked_List_Node *Linked_List_Node;
typedef struct type_Linked_List *Linked_List;
typedef struct type_Linked_List_Iterator *Linked_List_Iterator;

struct type_Linked_List_Node {
	Item item;
	Linked_List_Node next;
};

struct type_Linked_List {
	Linked_List_Node head;
	size_t item_count;
};

struct type_Linked_List_Iterator {
	Linked_List_Node next_node;
	Linked_List_Node curr_node;
};

Collection_Interface
collection_interface_linked_list();

Linked_List
collection_linked_list();

#endif
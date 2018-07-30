#include <stdio.h>
#include "value.h"
#include "talloc.h"

static Value *memory_head = NULL;
/*
 * A malloc-like function that allocates memory, tracking all allocated
 * pointers in the "active list."  (You can choose your implementation of the
 * active list, but whatever it is, your talloc code should NOT call functions
 * in linkedlist.h; instead, implement any list-like behavior directly here.
 * Otherwise you'll end up with circular dependencies, since you're going to
 * modify the linked list to use talloc instead of malloc.)
 */
void *talloc(size_t size){
	if(!memory_head){
		memory_head = malloc(sizeof(Value));
		memory_head->type = NULL_TYPE;
	}
	Value *new_value = malloc(sizeof(Value));
	void *address = malloc(size);
	new_value->type = CONS_TYPE;
	(new_value->c).car = address;
	(new_value->c).cdr = memory_head;
	memory_head = new_value;
	return address;
}

/*
 * Free all pointers allocated by talloc, as well as whatever memory you
 * malloc'ed to create/update the active list.
 */
void tfree(){
	Value *pt;
	while(memory_head->type != NULL_TYPE) {
        free((memory_head->c).car);
        pt = memory_head;
        memory_head = (memory_head->c).cdr;
        free(pt);
    }
    free(memory_head);
	memory_head = NULL;
}

/*
 * A simple two-line function to stand in the C function "exit", which calls
 * tfree() and then exit().  (You'll use this later to allow a clean exit from
 * your interpreter when you encounter an error: so memory can be automatically
 * cleaned up when exiting.)
 */
void texit(int status){
	tfree();
	exit(status);
}

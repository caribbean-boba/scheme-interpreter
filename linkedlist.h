#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include <stdbool.h>
#include "value.h"
/*
 * Create an empty list (a new Value object of type NULL_TYPE).
 */
Value *makeNull();

/*
 * Create a nonempty list (a new Value object of type CONS_TYPE).
 */
Value *cons(Value *car, Value *cdr);

/*
 * Print a representation of the contents of a linked list.
 */
void display(Value *list);

/*
 * Get the car value of a given list.
 * (Uses assertions to ensure that this is a legitimate operation.)
 */
Value *car(Value *list);

/*
 * Get the cdr value of a given list.
 * (Uses assertions to ensure that this is a legitimate operation.)
 */
Value *cdr(Value *list);

/*
 * Test if the given value is a NULL_TYPE value.
 * (Uses assertions to ensure that this is a legitimate operation.)
 */
bool isNull(Value *value);

/*
 * Compute the length of the given list.
 * (Uses assertions to ensure that this is a legitimate operation.)
 */
int length(Value *value);

/*
 * Create a new linked list whose entries correspond to the given list's
 * entries, but in reverse order.  The resulting list is a shallow copy of the
 * original: that is, stored data within the linked list should NOT be
 * duplicated; rather, the new list's (new) CONS_TYPE nodes should point to
 * precisely the items in the original list.
 *
 * (Uses assertions to ensure that this is a legitimate operation.)
 */
Value *reverse(Value *list);
#endif

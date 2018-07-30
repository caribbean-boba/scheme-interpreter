#include "linkedlist.h"
#include <stdlib.h>
#include <stdio.h>
#include "value.h"
#include <assert.h>
#include <stdbool.h>
#include "talloc.h"
/*
 * Create an empty list (a new Value object of type NULL_TYPE).
 */
Value *makeNull(){
    Value *val = talloc(sizeof(Value));
    val->type = NULL_TYPE;
    return val;
}

/*
 * Create a nonempty list (a new Value object of type CONS_TYPE).
 */
Value *cons(Value *car, Value *cdr){
    Value *node = talloc(sizeof(Value));
    node->type = CONS_TYPE;
    (node->c).car = car;
    (node->c).cdr = cdr;
    return node;
}

/*
 * Print a representation of the contents of a linked list.
 */
void display(Value *list){
    switch(list->type) {
        case INT_TYPE:
            printf("%i ", list->i);
            break;
        case DOUBLE_TYPE:
            printf("%f ", list->d);
            break;
        case STR_TYPE:
            printf("%s ", list->s);
            break;
        case CONS_TYPE:
            display((list->c).car);
            display((list->c).cdr);
            break;
        default:
            break;
    }
}

/*
 * Get the car value of a given list.
 * (Uses assertions to ensure that this is a legitimate operation.)
 */
Value *car(Value *list){
    assert(list != NULL);
    assert((list->c).car != NULL);
    assert(list->type == CONS_TYPE);
    return (list->c).car;
}

/*
 * Get the car value of a given list.
 * (Uses assertions to ensure that this is a legitimate operation.)
 */
Value *cdr(Value *list){
    assert(list != NULL);
    assert((list->c).cdr != NULL);
    assert(list->type == CONS_TYPE);
    return (list->c).cdr;
}

/*
 * Test if the given value is a NULL_TYPE value.
 * (Uses assertions to ensure that this is a legitimate operation.)
 */
bool isNull(Value *value){
    assert(value != NULL);
    if (value->type == NULL_TYPE) {
        return true;
    } else {
        return false;
    }
}

/*
 * Compute the length of the given list.
 * (Uses assertions to ensure that this is a legitimate operation.)
 */
int length(Value *value){
    assert(value != NULL);
    assert(value->type == CONS_TYPE || value->type == NULL_TYPE);
    int length = 0;
    while(value->type == CONS_TYPE){
        assert((value->c).cdr != NULL);
        value = (value->c).cdr;
        assert(value->type == CONS_TYPE || value->type == NULL_TYPE);
        length ++;
    }
    return length;
}

/*
 * reversehelper for reverse function
 */
Value *reverseHelper(Value *list, Value *result){
    if (isNull(list)){
        return result;
    } else {
        return reverseHelper(list->c.cdr, cons(car(list), result));
    }
}

/*
 * Create a new linked list whose entries correspond to the given list's
 * entries, but in reverse order.  The resulting list is a deep copy of the
 * original: that is, there should be no shared memory between the original
 * list and the new one.
 *
 * (Uses assertions to ensure that this is a legitimate operation.)
 *
 * FAQ: What if there are nested lists inside that list?
 * ANS: There won't be for this assignment. There will be later, but that will
 *      be after we've got an easier way of managing memory.
 */
Value *reverse(Value *list){
    assert(list != NULL);
    assert(list->type == CONS_TYPE || list->type  == NULL_TYPE);
    Value *result = reverseHelper(list, makeNull());
    return result;
}

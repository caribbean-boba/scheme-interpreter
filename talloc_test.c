#include <stdio.h>
#include "value.h"
#include "talloc.h"
#include "linkedlist.h"

int main() {
    Value *val1 = talloc(sizeof(Value));
    val1->type = INT_TYPE;
    val1->i = 12;

    Value *val2 = talloc(sizeof(Value));
    val2->type = DOUBLE_TYPE;
    val2->d = 4.3;

    Value *head = makeNull();
    head = cons(val1, head);
    head = cons(val2, head);
    display(head);
    printf("\n");
    head = reverse(head);
    display(head);
    tfree();
    printf("I can see this.\n");
    texit(1);
    Value *val5 = talloc(sizeof(Value));
    printf("I should never see this.\n");
}

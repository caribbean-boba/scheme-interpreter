/*Author: Xi Chen, Rosa Zhou, Yanhan Lyu*/
#include <stdio.h>
#include "value.h"
#include "talloc.h"
#include "linkedlist.h"
#include "tokenizer.h"

/*
* main function calling tokenize
*/
int main(void) {
   Value *list = tokenize();
   displayTokens(list);
   tfree();
   return 0;
}

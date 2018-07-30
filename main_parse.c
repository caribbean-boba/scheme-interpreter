#include <stdlib.h>
#include <stdio.h>
#include "value.h"
#include <assert.h>
#include <stdbool.h>
#include "talloc.h"
#include <string.h>
#include "tokenizer.h"
#include "parser.h"

int main(void) {
   Value *list = tokenize();
   Value *tree = parse(list);
   printTree(tree);
   printf("\n");
   tfree();
   return 0;
}

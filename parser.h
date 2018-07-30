#ifndef PARSER_H
#define PARSER_H
#include <stdlib.h>
#include "value.h"

// Take a linked list of tokens from a Scheme program
// Return a pointer to a parse tree representing the program
Value *parse(Value *tokens);

// Display the parse tree to the screen
// Using parentheses to denote tree structure
void printTree(Value *tree);

#endif

/*Author: Xi Chen, Rosa Zhou, Yanhan Lyu*/
#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <stdlib.h>
#include "value.h"
#include "talloc.h"

/* Read stdin (the standard input stream in C) in its entirety
* return a linked list consisting of all tokens found.
*/
Value *tokenize();

/* Takes a linked list of tokens as input,
*ndisplays those tokens, one per line, with each token's type.
*/
void displayTokens(Value *list);
#endif

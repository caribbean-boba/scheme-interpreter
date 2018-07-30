#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "value.h"

struct Frame {
   struct Value *bindings;
   struct Frame *parent;
};
typedef struct Frame Frame;

void interpret(struct Value *tree);
struct Value *eval(struct Value *expr, Frame *frame);

#endif

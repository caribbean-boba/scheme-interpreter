#ifndef VALUE_H
#define VALUE_H
#include "interpreter.h"


typedef enum {
   PTR_TYPE,
   INT_TYPE,
   DOUBLE_TYPE,
   STR_TYPE,
   CONS_TYPE,
   NULL_TYPE,
   OPEN_TYPE,
   CLOSE_TYPE,
   SYMBOL_TYPE,
   BOOL_TYPE,
   VOID_TYPE,
   PRIMITIVE_TYPE,
   CLOSURE_TYPE
} valueType;

struct Value {
   valueType type;
   union {
      void *p;
      int i;
      double d;
      char ch;
      char *s;
      struct Value *(*pf)(struct Value *);
      struct Closure {
        struct Value *parameters;
        struct Value *body;
        struct Frame *frame;
      } closure;
      struct ConsCell {
         struct Value *car;
         struct Value *cdr;
      } c;
   };
};

typedef struct Value Value;


#endif

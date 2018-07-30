#include "interpreter.h"
#include <stdlib.h>
#include <stdio.h>
#include "talloc.h"
#include "linkedlist.h"
#include "parser.h"
#include <string.h>
#include "value.h"
#include "tokenizer.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>


static Frame *top = NULL;
int MULTIPLY = 2;
int SUBTRACT = 0;
int ADD = 1;
int DIVIDE = 3;


/* A function that looks up the symbol recursively
 * Call evaluation error if not found
 * Otherwise return the found value
 */
Value *lookUpSymbol(Value *symbol, Frame *frame){
  Frame *curFrame = frame;
  if(curFrame == NULL){
    printf ("Undefined symbox: %s\n", symbol->s);
    texit(1);
  }
  Value *bindings = curFrame->bindings;
  while(bindings->type != NULL_TYPE){
    if (car(car(bindings))->type != SYMBOL_TYPE) {
      fprintf (stderr, "error at %s (%d)\n", __FUNCTION__, __LINE__);
      texit(1);
    }
    if(!strcmp(car(car(bindings))->s,symbol->s)){
      return cdr(car(bindings));
    }
    bindings = cdr(bindings);
  }
  if(curFrame->parent== NULL){
    printf ("Undefined symbox: %s\n", symbol->s);
    texit(1);
  }
  /* lookup symbol recursively */
  return lookUpSymbol(symbol, frame->parent);
}

/* A function that checks if the value if false */
int checkFalse(Value *val){
  if(val->type == BOOL_TYPE && val->ch == 'f'){
    return 1;
  }
  return 0;
}

/* A function that handles cond statement */
Value *evalCond(Value *tree, Frame *frame){
    Value *val = makeNull();
    val->type = VOID_TYPE;
    Value *test;
    while(tree->type != NULL_TYPE){
        if(car(tree)->type != CONS_TYPE || length(car(tree)) < 1){
            fprintf (stderr, "error at %s (%d)\n", __FUNCTION__, __LINE__);
            texit(1);
        }
        test = car(car(tree));
        if(test->type == SYMBOL_TYPE && strcmp(test->s,"else") == 0){
            Value *cur = cdr(car(tree));
            Value *result;
            while (cur->type != NULL_TYPE){
                result = eval(car(cur), frame);
                cur = cdr(cur);
            }
            return result;
        }
        /*if not false*/
        if(checkFalse(eval(test, frame)) == 0){
            if (length(car(tree)) == 1){
                return eval(test, frame);
            }
            Value *cur = cdr(car(tree));
            Value *result;
            while (cur->type != NULL_TYPE){
                result = eval(car(cur), frame);
                cur = cdr(cur);
            }
            return result;
        }
        tree = cdr(tree);
    }
    return val;
}

/* A function that handles let statement */
Value *evalIf(Value *tree, Frame *frame){
  /* if length not 2 or 3, call evaluation error */
  if(length(tree) != 3 && length(tree) != 2){
    printf ("If statement should contain at least two arguments");
    texit(1);;
  } else {
    Value *val = eval(car(tree), frame);
    if (length(tree) == 3){
      /* if false */
      if(val->type == BOOL_TYPE && val->ch == 'f'){
        /* return second value */
        return eval(car(cdr(cdr(tree))), frame);
      }else{
        return eval(car(cdr(tree)), frame);
       }
    } else {
      /* alternate specified here*/
      Value *alternate = talloc(sizeof(Value));
      alternate->type = VOID_TYPE;
      if(val->type == BOOL_TYPE && val->ch == 'f'){
        return alternate;
      } else {
        return eval(car(cdr(tree)), frame);
      }
    }
  }
  return NULL;
}

/* a function that handles division */
double handleDivision(double i, int val){
  if(val==0){
    printf ("Division by zero is not allowed\n");
    texit(1);;
  }
  return i/val;
}

Value *primitiveLeq(Value *args){
  if(length(args) < 2){
    printf ("Evaluation Error: Should contain at least two arguments\n");
    texit(1);;
  }
  Value *val = makeNull();
  val->type = BOOL_TYPE;
  val->ch = 't';
  int first = 1;
  double prev;
  double cur;
  while(args->type != NULL_TYPE){
    if(car(args)->type == INT_TYPE){
      cur = car(args)->i;
    }else if(car(args)->type == DOUBLE_TYPE){
      cur = car(args)->d;
    }else{
      fprintf (stderr, "error at %s (%d)\n", __FUNCTION__, __LINE__);
      texit(1);;
    }
    if(first){
      first = 0;
    }else{
      if(cur < prev){
        val->ch = 'f';
        return val;
      }
    }
    prev = cur;
    args = cdr(args);
  }
  return val;
}

bool fequal(double a, double b){
    return fabs(a-b) < 0.0001;
}

/*Primitive function for eq?*/
Value *primitiveEq(Value *args){
    Value *result = makeNull();
    result->type = BOOL_TYPE;
    result->ch = 'f';
    if(length(args) != 2){
      printf ("Evaluation Error: Should contain at least two arguments\n");
      texit(1);;
    }
    Value *firstVal = car(args);
    Value *secondVal = car(cdr(args));
    if(firstVal->type != secondVal->type){
      result->ch = 'f';
      return result;
    }
    switch (firstVal->type) {
        case(INT_TYPE):
            if (firstVal->i == secondVal->i) {
                result->ch = 't';
            }
            break;
        case(BOOL_TYPE):
            if (firstVal->ch == secondVal->ch) {
                result->ch = 't';
            }
            break;
        case(DOUBLE_TYPE):
            if (fequal(firstVal->d, secondVal->d)) {
                result->ch = 't';
            }
            break;
        case(CONS_TYPE):
            if (firstVal == secondVal) {
                result->ch = 't';
            }
            if(car(firstVal)->type == NULL_TYPE &&
                car(secondVal)->type == NULL_TYPE){
                result->ch = 't';
            }
            break;
        case(PRIMITIVE_TYPE):
            if (firstVal == secondVal) {
                result->ch = 't';
            }
            break;
        case(STR_TYPE):
            if (strcmp(firstVal->s, secondVal->s) == 0) {
                result->ch = 't';
            }
            break;
        case(SYMBOL_TYPE):
            if (strcmp(firstVal->s, secondVal->s) == 0) {
                result->ch = 't';
            }
            break;
        case(NULL_TYPE):
            result->ch = 't';
            break;
        case(CLOSURE_TYPE):
            if(firstVal == secondVal){
              result->ch = 't';
              break;
            }
        default:
            result->ch = 'f';
            break;
    }
    return result;
}

// This function handles of primitive arithmetic
Value *handleEdgeCase(Value *args, int n){
  if((n == DIVIDE || SUBTRACT)&& length(args) == 0){
    printf ("Evaluation Error: Missing arguments\n");
    texit(1);;
  }
  if((n == SUBTRACT || DIVIDE)&& length(args) == 1){
    Value *zero = makeNull();
    zero->type = INT_TYPE;
    if(n == DIVIDE){
      zero->i = 1;
    }else{
      zero->i = 0;
    }
    return cons(zero,args);
  }
  return args;
}

/* This function adds up all arguments */
Value *primitiveArithmetic(Value *args, int n){
    float i;
    if(n == ADD || n == SUBTRACT){
      i = 0;
    }else{
      i = 1;
    }
   args = handleEdgeCase(args,n);
   Value *cur;
   bool isDouble = false;
   int first = 1;
   while(args->type != NULL_TYPE){
     cur = car(args);
     if(cur->type == CONS_TYPE && length(cur) == 1){
       cur = car(cur);
     }
     double val;
     if(cur->type == INT_TYPE){
       val = cur->i;
     }else if(cur ->type == DOUBLE_TYPE){
       val = cur->d;
       isDouble = true;
     }else{
        printf ("Arithmetic operation only on numbers\n");
        texit(1);;
     }
     if((first && n == SUBTRACT)|| n == ADD){
       first = 0;
       i += val;
     }else if (n == SUBTRACT){
       i -= val;
     }else if((first && n == DIVIDE)|| n == MULTIPLY){
       first = 0;
       i *= val;
     }else{
       i = handleDivision(i, val);
     }
     args = cdr(args);
   }
   Value *val = makeNull();
    /* Handles printed form */
    if (isDouble || n == DIVIDE) {
        val->type = DOUBLE_TYPE;
        val->d = i;
    } else {
        val->type = INT_TYPE;
        val->i = i;
    }
   return val;
}

// This function adds up any number of arguments
Value *primitiveAdd(Value *args){
  return primitiveArithmetic(args, ADD);
}
// This function subtracts any number of arguments
Value *primitiveSubtract(Value *args){
  return primitiveArithmetic(args, SUBTRACT);
}
// This function multiplys any number of arguments
Value *primitiveMultiply(Value *args){
  return primitiveArithmetic(args, MULTIPLY);
}
// This function divides any number o arguments
Value *primitiveDivide(Value *args){
  return primitiveArithmetic(args, DIVIDE);
}

/* This function returns true if args if null */
Value *primitiveNull(Value *args){
  if(length(args) != 1){
    printf ("Evaluation Error: null? should contain at least one argument");
    texit(1);;
  }
  Value *val = makeNull();
  val->type = BOOL_TYPE;
  if(car(args)->type != CONS_TYPE){
    val->ch = 'f';
  }else{
    args = car(args);
    if(isNull(car(args))){
      val->ch = 't';
    }else{
      val->ch = 'f';
    }
  }
  return val;
}

/* This function checks the input of cdr and car */
void checkInputCarCdr(Value *args){
  if(length(args) != 1){
    printf ("Evaluation Error: Missing arguments");
    texit(1);;
  }
  if(car(args)->type != CONS_TYPE){
    printf ("Evaluation Error: should be in cons type");
    texit(1);;
  }
  if(car(car(args))->type != CONS_TYPE){
    printf ("Evaluation Error: should be in cons type");
    texit(1);;
  }
}

/* This function returns the car of the argument */
Value *primitiveCar(Value *args){
  checkInputCarCdr(args);
  args = car(args);
  Value *val = cons(car(car(args)), makeNull());
  if(car(val)->type != CONS_TYPE){
    val = car(val);
  }
  return val;
}


/* This function returns the cdr of the argument */
Value *primitiveCdr(Value *args){
  checkInputCarCdr(args);
  Value *new = makeNull();
  Value *new2 = cdr(car(args)->c.car);
  new = cons(new2, new);
  if(car(new)->type != CONS_TYPE
    && car(new)->type != NULL_TYPE
    && length(new) == 1){
    new = car(new);
  }
  return new;
}

/* This function checks if the input is a pair */
Value *primitivePair(Value *args){
  Value *result = makeNull();
  result->type = BOOL_TYPE;
  result->ch = 'f';
  if(length(args) != 1){
    printf ("Evaluation Error: pair? should contain only one cons type argument\n");
    texit(1);;
  }
  if(car(args)->type == CONS_TYPE){
    if(car(car(args))->type != NULL_TYPE){
      result->ch = 't';
    }
  }
  return result;
}

/* This function returns the cons of two arguments */
Value *primitiveCons(Value *args){
  if(length(args) != 2){
    printf ("Evaluation Error: cons should contain two arguments\n");
    texit(1);;
  }
  Value *first = car(args);
  Value *second = car(cdr(args));
  if(first->type == CONS_TYPE){
    first = car(first);
  }
  if(second->type == CONS_TYPE){
    second = car(second);
  }
  Value *val = cons(first, second);
  val = cons(val, makeNull());
  return val;
}

/* A function that handles let star*/
Value *handleLetStar(Value *tree){
  Value *first = car(car(tree));
  first = cons(first, makeNull());
  Value *remaining = cdr(car(tree));
  remaining = cons(remaining, cdr(tree));
  Value *letStar = makeNull();
  letStar->type = SYMBOL_TYPE;
  letStar->s = "let*";
  remaining = cons(letStar, remaining);
  tree = cons(first, cons(remaining, makeNull()));
  return tree;
}

/* A function that handles if statement */
/* letrec and let star only support single body expression */
Value *evalLet(Value *tree, Frame *frame, int i){
    if (length(tree) < 2) {
        printf ("Evaluation Error: Missing arguments\n");
        texit(1);;
    }
    if(tree->type != CONS_TYPE){
        fprintf (stderr, "error at %s (%d)\n", __FUNCTION__, __LINE__);
        texit(1);;
    }
    if(i == 2){
      if(car(tree)->type == NULL_TYPE){
        return eval(car(cdr(tree)),frame);
      }
      tree = handleLetStar(tree);
    }
    Frame *newFrame = talloc(sizeof(Frame));
    newFrame->parent = frame;
    Value *bindings = makeNull();
    newFrame->bindings = bindings;
    Value *innerTree = car(tree);
    Value *curTree;
    while(innerTree->type != NULL_TYPE){
        Value *head = bindings;
        curTree = car(innerTree);
        if(curTree->type != CONS_TYPE ||
           car(curTree)->type != SYMBOL_TYPE){
            fprintf (stderr, "error at %s (%d)\n", __FUNCTION__, __LINE__);
            texit(1);
        }
        /* evaluation error if duplicates in let */
        Value *tempBindings = head;
        while(tempBindings->type != NULL_TYPE) {
            Value *temp = car(tempBindings);
            if (!strcmp(car(curTree)->s, car(temp)->s)) {
                printf ("Evaluation Error: duplicates in let expression");
                texit(1);
            }
            tempBindings = cdr(tempBindings);
        }
        Value *bind = makeNull();
        bind->type = CONS_TYPE;
        bind->c.car = curTree->c.car;
        if(i == 0){
          (bind->c).cdr = eval(car(cdr(curTree)), frame);
        }else{
          (bind->c).cdr = eval(car(cdr(curTree)), newFrame);
        }
        bindings = cons(bind, bindings);
        innerTree = cdr(innerTree);
    }
    /* create a new frame */
    newFrame->bindings = bindings;
    Value *cur = cdr(tree);
    Value *result;
    /* evaluate each body in the new frame only return the last one */
    while (cur->type != NULL_TYPE){
        result = eval(car(cur), newFrame);
        cur = cdr(cur);
    }
    return result;
}


/* A function that handles quote statement */
Value *evalQuote(Value *tree) {
    if (tree->type != CONS_TYPE) {
        fprintf (stderr, "error at %s (%d)\n", __FUNCTION__, __LINE__);
        texit(1);
    } else if (cdr(tree)->type != NULL_TYPE) {
        fprintf (stderr, "error at %s (%d)\n", __FUNCTION__, __LINE__);
        texit(1);
    } else {
        if(car(tree)->type == CONS_TYPE || car(tree)->type == NULL_TYPE){
          return tree;
        }
        return car(tree);
    }
    return NULL;
}

/* A function that handles define statement */
Value *evalDefine(Value *tree, Frame *frame) {
    if (length(tree) != 2 || car(tree)->type != SYMBOL_TYPE) {
        printf ("Evaluation Error: define should have length 2 and the first argument should be a symbol\n");
        texit(1);;
    }
    if(frame->parent != NULL){
      printf ("Evaluation error: Definitions cannot be last in body.");
      texit(1);
    }
    Value *bindings = frame->bindings;
    Value *binding = talloc(sizeof(Value));
    binding->type = CONS_TYPE;
    binding->c.car = car(tree);
    binding->c.cdr = eval(car(cdr(tree)), frame);
    bindings = cons(binding, bindings);
    top->bindings = bindings;
    Value *result = talloc(sizeof(Value));
    result->type = VOID_TYPE;
    return result;
}

// This function evaluates begin
Value *evalBegin(Value *tree, Frame *frame) {
    if(tree->type == CONS_TYPE && car(tree)->type == NULL_TYPE){
      printf("Evaluation Error: begin missing argument\n");
      texit(1);
    }
    Value *cur = tree;
    Value *result = makeNull();
    result->type = VOID_TYPE;
    while (cur->type != NULL_TYPE){
        result = eval(car(cur), frame);
        cur = cdr(cur);
    }
    return result;
}

/* A function that handles lambda */
Value *evalLambda(Value *tree, Frame *frame) {
    if(length(tree) < 2){
      printf ("Evaluation Error: lambda missing arguments");
      texit(1);
    }
    if(car(tree)->type != SYMBOL_TYPE){
      if(car(tree)->type != CONS_TYPE
         && car(tree)->type != NULL_TYPE
         && car(tree)-> type != SYMBOL_TYPE){
        fprintf (stderr, "error at %s (%d)\n", __FUNCTION__, __LINE__);
        texit(1);
      }
      Value *head = car(tree);
      while(head->type != NULL_TYPE){
        if(car(head)->type != SYMBOL_TYPE){
          fprintf (stderr, "error at %s (%d)\n", __FUNCTION__, __LINE__);
          texit(1);
        }
        head = cdr(head);
      }
    }
    Value *val = talloc(sizeof(Value));
    val->type = CLOSURE_TYPE;
    val->closure.frame = frame;
    val->closure.body = car(cdr(tree));
    val->closure.parameters = car(tree);
    return val;
}

/* A function binds the primitive function to the top frame */
void bind(char *name, Value *(*function)(Value *), Frame *frame) {
   Value *value = makeNull();
   value->type = PRIMITIVE_TYPE;
   value->pf = function;
   Value *function_name = makeNull();
   function_name->type = SYMBOL_TYPE;
   function_name->s = name;
   Value *binding = cons(function_name,value);
   frame->bindings = cons(binding, frame->bindings);
}

/* A function that handles variadic argument */
Value *handleVariadic(Value *binding, Value *parameters, Value *args){
  Value *list = makeNull();
  while(args->type != NULL_TYPE){
    Value *val = car(args);
    if(val->type == CONS_TYPE){
      val = car(val);
    }
    list = cons(val, list);
    args = cdr(args);
  }
  list = reverse(list);
  list = cons(list, makeNull());
  Value *bind = cons(parameters, list);
  return cons(bind, binding);
}

/* A function that applies the function to the args */
Value *apply(Value *function, Value *args){
  if (function->type != CLOSURE_TYPE && function->type != PRIMITIVE_TYPE) {
    printf ("Evaluation Error: apply only works on function type");
    texit(1);;
  }
  if (function->type == PRIMITIVE_TYPE) {
    return (function->pf)(args);
  }
  Value *parameters = function->closure.parameters;
  Value *bindings = makeNull();
  if(parameters->type == SYMBOL_TYPE){
    bindings = handleVariadic(bindings, parameters, args);
  }else if(length(args) == length(parameters)){
    while(args->type != NULL_TYPE){
      Value *binding = cons(car(parameters), car(args));
      bindings = cons(binding, bindings);
      parameters = cdr(parameters);
      args = cdr(args);
    }
  }else{
    fprintf (stderr, "error at %s (%d)\n", __FUNCTION__, __LINE__);
    texit(1);;
  }
  Frame *newFrame = talloc(sizeof(Frame));
  newFrame->parent = function->closure.frame;
  newFrame->bindings = bindings;
  return eval(function->closure.body, newFrame);
}

/* A function that handles combined statement */
Value *evalComb(Value *tree, Frame *frame) {
  Value *function = eval(car(tree), frame);
  tree = cdr(tree);
  Value *args = makeNull();
  while(tree->type != NULL_TYPE){
    args = cons(eval(car(tree), frame),args);
    tree = cdr(tree);
  }
  args = reverse(args);
  return apply(function, args);
}

/* A function that handles and */
Value *evalAnd(Value *tree, Frame *frame) {
  Value *args = makeNull();
  args->type = BOOL_TYPE;
  args->ch = 't';
  while(tree->type != NULL_TYPE){
    args = eval(car(tree),frame);
    if(args->type==BOOL_TYPE && args->ch=='f'){
      return args;
    }
    tree = cdr(tree);
  }
  return args;
}

/* A function that handles or */
Value *evalOr(Value *tree, Frame *frame) {
  Value *args = makeNull();
  args->type = BOOL_TYPE;
  args->ch = 'f';
  while(tree->type != NULL_TYPE){
    args = eval(car(tree),frame);
    if(args->type!=BOOL_TYPE || args->ch!='f'){
      return args;
    }
    tree = cdr(tree);
  }
  return args;
}

/*Extension: load. Open the file and evaluate the code inside it.*/
Value *evalLoad(Value *file, Frame *frame) {
    Value *returnVoid = makeNull();
    returnVoid->type = VOID_TYPE;
    if (length(file) == 1) {
        file = eval(car(file), frame);
        if (file->type != STR_TYPE){
          fprintf (stderr, "error at %s (%d)\n", __FUNCTION__, __LINE__);
          texit(1);;
        }else{
          char *filename = file->s;
          FILE *file_open = fopen(filename, "r");
          if (file_open != NULL) {
            Value *list = tokenize(file_open);
            fclose(file_open);
            Value *tree = parse(list);
            interpret(tree);
          } else {
            printf("%s does not exist.\n", filename);
            fprintf (stderr, "error at %s (%d)\n", __FUNCTION__, __LINE__);
            texit(1);;
          }
        }
      return returnVoid;
    } else {
       fprintf (stderr, "error at %s (%d)\n", __FUNCTION__, __LINE__);
        texit(1);;
       return NULL;
    }
}

/* A function that handles Set! */
Value *evalSet(Value *tree, Frame *frame) {
  Value *bindings = frame->bindings;
  Value *cur;
  if(length(tree) != 2){
    printf ("Evaluation Error: set! should contain 2 arguments\n");
    fprintf (stderr, "error at %s (%d)\n", __FUNCTION__, __LINE__);
    texit(1);
  }
  Value *symbol = car(tree);
  if(symbol->type != SYMBOL_TYPE){
    printf ("Evaluation Error: The first argument in set! should be a symbol type\n");
    fprintf (stderr, "error at %s (%d)\n", __FUNCTION__, __LINE__);
    texit(1);
  }
  Value *empty = makeNull();
  empty->type = VOID_TYPE;
  if (bindings != NULL && bindings->type != NULL_TYPE){
    cur = car(bindings);
  }
  while(bindings->type != NULL_TYPE){
    cur = car(bindings);
    if(!strcmp(car(cur)->s, symbol->s)){
      cur->c.cdr = eval(car(cdr(tree)),frame);
      return empty;
    }
    bindings = cdr(bindings);
  }
  if(frame->parent == NULL){
    printf ("Evaluation Error: Undefined symbol: %s\n", symbol->s);
    texit(1);
  }
  return evalSet(tree, frame->parent);
}

/* A function handles symbol */
Value *evalSymbol(Value *tree, Frame *frame){
  if (!strcmp(car(tree)->s, "if")) {
    return evalIf(cdr(tree), frame);
  }else if(!strcmp(car(tree)->s, "or")) {
    return evalOr(cdr(tree), frame);
  }else if(!strcmp(car(tree)->s, "and")) {
    return evalAnd(cdr(tree), frame);
  }else if(!strcmp(car(tree)->s, "set!")) {
    return evalSet(cdr(tree), frame);
  }else if(!strcmp(car(tree)->s, "cond")) {
    return evalCond(cdr(tree), frame);
  }else if (!strcmp(car(tree)->s, "begin")) {
    return evalBegin(cdr(tree), frame);
  }else if (!strcmp(car(tree)->s, "quote")){
    return evalQuote(cdr(tree));
  }else if (!strcmp(car(tree)->s, "let")){
    return evalLet(cdr(tree), frame, 0);
  }else if (!strcmp(car(tree)->s, "letrec")){
    return evalLet(cdr(tree), frame, 1);
  }else if (!strcmp(car(tree)->s, "let*")){
    return evalLet(cdr(tree), frame, 2);
  }else if (!strcmp(car(tree)->s, "define")){
    return evalDefine(cdr(tree), frame);
  }else if (!strcmp(car(tree)->s, "lambda")){
    return evalLambda(cdr(tree), frame);
  }else if (!strcmp(car(tree)->s, "load")) {
    return evalLoad(cdr(tree), frame);
  }else{
    return evalComb(tree, frame);
  }
}

/* Evaluate the value in a given frame */
Value *eval(Value *tree, Frame *frame) {
  switch (tree -> type) {
    case INT_TYPE:
      return tree;
      break;
    case SYMBOL_TYPE:
      return lookUpSymbol(tree, frame);
      break;
    case DOUBLE_TYPE:
      return tree;
      break;
    case BOOL_TYPE:
      return tree;
      break;
    case STR_TYPE:
      return tree;
      break;
    case CONS_TYPE:
      if(car(tree)->type == SYMBOL_TYPE){
        return evalSymbol(tree, frame);
      }else{
        return evalComb(tree, frame);
      }
      break;
    default:
      break;
  }
  return NULL;
}

/* This function binds all primitives to the top frame */
void bindPrimitive(Frame *top){
  bind("+", primitiveAdd, top);
  bind("*", primitiveMultiply, top);
  bind("/", primitiveDivide, top);
  bind("-", primitiveSubtract, top);
  bind("null?", primitiveNull, top);
  bind("car", primitiveCar, top);
  bind("cdr", primitiveCdr, top);
  bind("pair?", primitivePair, top);
  bind("cons", primitiveCons, top);
  bind("<=", primitiveLeq, top);
  bind("eq?", primitiveEq, top);
}

/* interpret a program */
void interpret(Value *tree){
  if (top == NULL){
    top= talloc(sizeof(Frame));
    top->parent = NULL;
    top->bindings = makeNull();
    bindPrimitive(top);
  }
  /* evaluate each value in the linkedlist */
  while(tree->type != NULL_TYPE){
      Value *val = eval(car(tree),top);
      if(val -> type != VOID_TYPE){
        printTree(val);
        printf("\n");
      }
      tree = cdr(tree);
  }
}

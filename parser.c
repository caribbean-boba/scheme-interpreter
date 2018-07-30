// Author: Rosa Zhou, Yanhan Lyu, Xi Chen
#include <stdlib.h>
#include <stdio.h>
#include "value.h"
#include <assert.h>
#include <stdbool.h>
#include "talloc.h"
#include <string.h>
#include "tokenizer.h"
#include "linkedlist.h"
#include "parser.h"

/* A function that adds a token the the parse tree
* Returns an updated tree, and update depth
*/
Value *addToParseTree(Value *tree, int *depth, Value *token){
    // if open parentheses, increase depth
    if(token->type == OPEN_TYPE){
      (*depth)++;
      tree = cons(token, tree);
    }else if(token->type == CLOSE_TYPE){
      Value *current = tree;
      Value *token1;
      // create subtree
      Value *subTree = makeNull();
      // handle when there is only ) in the token list
      if (current->type != NULL_TYPE) {
        token1 = car(tree);
      } else {
        printf("Syntax error: too many close parentheses.\n");
        texit(1);
      }
      // add to subtree until reaching an open paren
      while(token1->type != OPEN_TYPE){
        subTree = cons(token1, subTree);
        current = cdr(current);
        // if reach end of list ,report error
        if(current->type == NULL_TYPE){
          printf("Syntax error: too many close parentheses.\n");
          texit(1);
        }else{
          token1 = car(current);
          // pop the token of the tree
          tree = cdr(tree);
        }
      }
      // pop the open parentheses off the tree
      tree = cdr(tree);
      (*depth)--;
      tree = cons(subTree, tree);
    }else{
      tree = cons(token, tree);
    }
    return tree;
}

/*a helper to print string correctly*/
void stringHelper(char *s){
    printf ("\"");
    int i = 0;
    while (s[i] != '\0'){
        if (s[i] == '\n'){
            printf ("\\n");
        } else if (s[i] == '\t'){
            printf ("\\t");
        } else if (s[i] == '\\'){
            printf ("\\\\");
        } else if(s[i] == '\"'){
            printf ("\\\"");
        } else {
            printf ("%c", s[i]);
        }
        i++;
    }
    printf ("\"");
}

/* A helper function that prints out a single token*/
void printToken(Value *token, int end, int first){
  switch(token->type) {
      case INT_TYPE:
        printf("%i", token->i);
        break;
      case DOUBLE_TYPE:
        printf("%f", token->d);
        break;
      case STR_TYPE:
        // Correctly format the string
        stringHelper(token->s);
        break;
      case BOOL_TYPE:
        printf("#%c", token->ch);
        break;
      case SYMBOL_TYPE:
        printf("%s", token->s);
        break;
      case NULL_TYPE:
        printf("()");
        break;
      case CLOSURE_TYPE:
        printf("#<procedure>");
        break;
      case PRIMITIVE_TYPE:
        printf("#<procedure>");
        break;
      default:
        break;
  }
  if(!end && !first){
    printf(" ");
  }
}

/* A helper function for printTree
*  correctly handles white space and parentheses
*/
void printTreeHelper(Value *tree, int end, int first){
  if(tree->type != CONS_TYPE){
    if(!(tree->type == NULL_TYPE && first)){
      printToken(tree, end, first);
    }
  }else{
    // if depth 1, don't print out (
    if(!first){
      printf("(");
    }
    // recursively print out nested lists
    while(tree->type == CONS_TYPE){
      if(cdr(tree)->type != NULL_TYPE){
        printTreeHelper(car(tree), 0, 0);
      }else{
        printTreeHelper(car(tree), 1, 0);
      }
      tree = cdr(tree);
    }
    if(tree->type != NULL_TYPE){
      printf(". ");
      printTreeHelper(tree, 1, 0);
    }
    // if depth 1, don't print out )
    if(!first){
      printf(")");
    }
    // if not end of list or inner list, print out " "
    if(!end && !first){
      printf(" ");
    }
  }
}

/* Display the parse tree to the screen
* Using parentheses to denote tree structure
*/
void printTree(Value *tree){
  printTreeHelper(tree, 0, 1);
}

Value *handleQuote(Value *tree, int *depth){
  Value *open = makeNull();
  open->type = OPEN_TYPE;
  tree = addToParseTree(tree, depth, open);
  Value *quote = makeNull();
  quote->type = SYMBOL_TYPE;
  quote->s = "quote";
  return addToParseTree(tree, depth, quote);
}

/* Take a linked list of tokens from a Scheme program
*Return a pointer to a parse tree representing the program
*/
Value *parse(Value *tokens) {
   Value *tree = makeNull();
   int depth = 0;
   int quote = 0;
   Value *depths = makeNull();
   Value *current = tokens;
   assert(current != NULL && "Error (parse): null pointer");
   while (current->type != NULL_TYPE) {
      Value *token = car(current);
      if(token->type == SYMBOL_TYPE && !strcmp(token->s,"\'")){
        Value *cur = makeNull();
        cur->i = depth;
        depths = cons(cur,depths);
        tree = handleQuote(tree, &depth);
      }else{
        tree = addToParseTree(tree, &depth, token);
        if(depths->type != NULL_TYPE && depth == car(depths)->i+1){
          depths = cdr(depths);
          Value *close = makeNull();
          close->type = CLOSE_TYPE;
          tree = addToParseTree(tree, &depth, close);
        }
      }
      current = cdr(current);
   }
   // if depth greater than 1, report error
   if (depth != 0) {
     printf("Syntax error: not enough close parentheses. \n");
     texit(1);
   }
   return reverse(tree);
}

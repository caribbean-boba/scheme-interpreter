/*Author: Xi Chen, Rosa Zhou, Yanhan Lyu*/
#include "linkedlist.h"
#include <stdlib.h>
#include <stdio.h>
#include "value.h"
#include <assert.h>
#include <stdbool.h>
#include "talloc.h"
#include <string.h>

char number[] = "1234567890";
char letter[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
char special[] = "!$&*.:<=>?/_^%~";
char sign[] ="+-";
char all[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!$&*.:<=>?/_^%~+-1234567890";
char num_initial[] = "+-.1234567890";
int size = 256;
FILE *stream;

/* This fucntion prints out an error message */
void errorMessage(char c){
    printf("Cannot recognize %c\n", c);
    texit(1);
}

/* This function append a character to a string*/
int appendChar(char*s, char c) {
     if(strlen(s) + 1 >= size) {
       printf("String length exceeds limit\n");
       texit(1);
       return 1;
     }
     int len = strlen(s);
     s[len] = c;
     s[len+1] = '\0';
     return 0;
}

/* This function extract a string from the input stream*/
int stringHandler(Value *value){
    // create a new string
    char *str = talloc(sizeof(char) * size);
    str[0] = '\0';
    char charRead = fgetc(stream);
    int escape = 0;
    while((charRead != '\"'|| escape) && charRead != EOF){
        // Handle escape characters
        if (escape) {
          escape = (escape+1)%2;
          if(strchr("'\"\\", charRead)){
            appendChar(str, charRead);
          }else if (charRead == 'n'){
            appendChar(str, '\n');
          }else if(charRead == 't'){
            appendChar(str, '\t');
          }else{
            errorMessage('\\');
          }
        }else if(charRead == '\\'){
          escape = (escape+1)%2;
        }else{
          appendChar(str,charRead);
        }
        charRead = fgetc(stream);
    }
    if(charRead == EOF){
        return 0;
    }
    value->type = STR_TYPE;
    value->s = str;
    return 1;
}


/* This function extract a number from the input stream*/
void numberHandler(Value *val){
    char *str = talloc(sizeof(char) * size);
    str[0] = '\0';
    char charRead = fgetc(stream);
    // read character until reaching one of )( \n;
    while(!strchr(")( \n;\t'", charRead) && charRead != EOF){
        appendChar(str, charRead);
        charRead = fgetc(stream);
    }
    ungetc(charRead,stream);
    // make sure dot can only appear once
    int dotCount = 0;
    for(int i = 0; i < strlen(str); i++){
        // make sure sign can only be located at the beginning
        if(strchr(sign, str[i]) && i != 0){
            errorMessage(str[i]);
        }else if(strchr(".", str[i])){
            // left side or right side of . must be a number
            int left = 0;
            int right = 0;
            if(i-1 >= 0 && strchr(number, str[i-1])){
                left = 1;
            }
            if(i+1 < strlen(str) && strchr(number, str[i+1])){
                right = 1;
            }
            if(left + right == 0){
                errorMessage(str[i]);
            }
            if(dotCount != 0){
                errorMessage(str[i]);
            }
            dotCount ++;
        }else if(!strchr(num_initial, str[i])){
            errorMessage(str[i]);
        }
    }
    // double if there is one dot
    if(dotCount != 0){
        val->type = DOUBLE_TYPE;
        val->d = atof(str);
    // int if no dot
    }else{
        val->type = INT_TYPE;
        val->i = atoi(str);
    }
}

/* This function extract a symbol from the input stream*/
void symbolHandler(Value *val){
    char *str = talloc(sizeof(char) * size);
    str[0] = '\0';
    char charRead = fgetc(stream);
    // read char until reaching  one of )( \n;\t
    while(!strchr(")( \n;\t\'", charRead) && charRead != EOF){
        appendChar(str, charRead);
        charRead = fgetc(stream);
    }
    ungetc(charRead, stream);
    // report error if char unexpected
    for(int i = 1; i < strlen(str); i++){
        if(!(strchr(all, str[i]))){
            errorMessage(str[i]);
        }
    }
    val->type = SYMBOL_TYPE;
    val->s = str;
}

/* print out a string and add the escape characters*/
void stringPrintHelper(char *s){
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
    printf(":string\n");
}

/* Takes a linked list of tokens as input,
*displays those tokens, one per line, with each token's type.
*/
void displayTokens(Value *list){
  switch(list->type) {
      case INT_TYPE:
        printf("%i:integer\n", list->i);
        break;
      case DOUBLE_TYPE:
        printf("%f:float\n", list->d);
        break;
      case STR_TYPE:
        stringPrintHelper(list->s);
        break;
      case BOOL_TYPE:
        printf("#%c:boolean\n", list->ch);
        break;
      case OPEN_TYPE:
        printf("(:open\n");
        break;
      case CLOSE_TYPE:
        printf("):close\n");
        break;
      case SYMBOL_TYPE:
        printf("%s:symbol\n", list->s);
        break;
      case CONS_TYPE:
        displayTokens((list->c).car);
        displayTokens((list->c).cdr);
        break;
      default:
        break;
  }
}


/* Read stream (the standard input stream in C) in its entirety
* return a linked list consisting of all tokens found.
*/
Value *tokenize(FILE *stream_g) {
    stream = stream_g;
    char charRead;
    // create a head
    Value *list = makeNull();
    charRead = fgetc(stream);
    Value *val;
    // read char until reaching EOF
    while (charRead != EOF) {
        val = talloc(sizeof(Value));
        val->type = NULL_TYPE;
        // handle comments
        if (charRead == ';'){
            while(charRead != '\n' && charRead != EOF){
                charRead = fgetc(stream);
            }
            continue;
        // handle boolean
      } else if(charRead == '\''){
        val->type = SYMBOL_TYPE;
        val->s = "\'";
      }else if(charRead == '#'){
            charRead = fgetc(stream);
            if (charRead == 't' || charRead == 'f'){
                val->type = BOOL_TYPE;
                val->ch = charRead;
            } else {
                errorMessage(charRead);
            }
        // handle open brackets
        } else if(charRead == '(') {
            val->type = OPEN_TYPE;
        // handle close brackets
        } else if (charRead == ')') {
            val->type = CLOSE_TYPE;
        // handle strings
        } else if (charRead == '"'){
            if(!stringHandler(val)){
                printf("Expecting a closing \"\n");
                texit(1);
            }
        // handle number
        } else if (strchr(num_initial, charRead)){
            char initial = charRead;
            charRead = fgetc(stream);
            // handle + and -, determine if they are symbols or beginning of a number
            if((strchr(" ()\n\t", charRead)||charRead == EOF) && strchr(sign, initial)){
                ungetc(charRead, stream);
                ungetc(initial, stream);
                symbolHandler(val);
            } else {
                ungetc(charRead, stream);
                ungetc(initial, stream);
                numberHandler(val);
            }
        // handle symbol
        } else if (strchr(letter, charRead) || (strchr(special, charRead))){
            ungetc(charRead, stream);
            symbolHandler(val);
        // report error if char not recognized
        } else if (!strchr("\n \t", charRead) && charRead != EOF){
                errorMessage(charRead);
        }else {
            charRead = fgetc(stream);
            continue;
        }
        list = cons(val,list);
        charRead = fgetc(stream);
    }
    return reverse(list);
}

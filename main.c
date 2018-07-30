#include "parser.h"
#include "interpreter.h"
#include "talloc.h"
#include "linkedlist.h"
#include <stdio.h>
#include "tokenizer.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int readInput(Value *list, Value *tree);
int main(int argc, char **argv) {
	Value *list = makeNull();
	Value *tree;
	FILE *file_open = fopen("build-in", "r");
	if (file_open != NULL) {
		list = tokenize(file_open);
		fclose(file_open);
		tree = parse(list);
		interpret(tree);
	} else {
		printf("%s does not exist.\n", "build-in");
		texit(1);
	}
    if (isatty(STDIN_FILENO)){
    	readInput(list, tree);
	 	return 0;
        //printf ("%s", buffer);
   }else{
	   list = tokenize(stdin);
	   tree = parse(list);
	   interpret(tree);
	   tfree();
	   return 0;
   	}
}

void append(char* s, char c)
{
    int len = strlen(s);
    s[len] = c;
    s[len+1] = '\0';
}

int readInput(Value *list, Value *tree){
  	FILE *file;
 	int left = 0;
 	int right = 0;
	char buffer;
	char str[1024];
	while(1){
		while (scanf("%c", &buffer) == 1){
			if (buffer != '\n'){
				if (buffer == '('){
					left ++;
				} else if (buffer == ')'){
					right ++;
				}
				//printf ("%lu\n", strlen(buffer));
				append(str, buffer);
				//printf ("%s", str);
			} else{
				file = fopen("records.txt", "wt");
				if (!file){
				   printf("File could not be opened\n");
				   return -1;
				}
				fprintf(file, "%s", str);
				//printf ("%s", str);
				fclose(file);
				if (left == right){
					file = fopen("records.txt", "r");
					if (!file) {
				    	printf("File could not be opened\n");
				    	return -1;
				 	} else {
				 		memset(str,0,sizeof(str));
				 		//count ++;
				 		//printf("%d",count);
			  			list = tokenize(file);
						fclose(file);
						tree = parse(list);
						interpret(tree);
						//tfree();
						//str[0] = '\0';
						remove("records.txt");
						left = 0;
						right = 0;
					}
				} else if (right > left){
					printf ("Too many close parathethis\n");
					return 0;
				}
			}
		}
	}
	tfree();
	return 1;
	
}



 




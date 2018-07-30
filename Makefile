CC = clang
CFLAGS = -g

SRCS = tokenizer.c linkedlist.c main.c talloc.c parser.c interpreter.c
HDRS = tokenizer.h linkedlist.h value.h talloc.h parser.h interpreter.h
OBJS = $(SRCS:.c=.o)

interpreter: $(OBJS)
	$(CC)  $(CFLAGS) $^  -o $@

memtest: interpreter
	valgrind --leak-check=full --show-leak-kinds=all ./$<

%.o : %.c $(HDRS)
	$(CC)  $(CFLAGS) -c $<  -o $@

clean:
	rm *.o
	rm interpreter

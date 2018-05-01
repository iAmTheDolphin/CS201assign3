OBJS = bst.o prim.o integer.o queue.o sll.o stack.o dll.o real.o scanner.o string.o gst.o avl.o vertex.o edge.o binomial.o
OOPTS = -Wall -Wextra -std=c99 -g -c
LOPTS = -Wall -Wextra -g
EXEC = ./prim testfile

all : prim

prim : $(OBJS)
		gcc $(LOPTS) $(OBJS) -o prim -lm

string.o : string.c string.h
		gcc $(OOPTS) string.c

binomial.o : binomial.c binomial.h dll.o 
		gcc $(OOPTS) binomial.c

vertex.o : vertex.c vertex.h dll.o
		gcc $(OOPTS) vertex.c

edge.o : edge.c edge.h
		gcc $(OOPTS) edge.c

scanner.o : scanner.c scanner.h
		gcc $(OOPTS) scanner.c

real.o : real.c real.h
		gcc $(OOPTS) real.c

integer.o : integer.c integer.h
		gcc $(OOPTS) integer.c

sll.o : sll.c sll.h
		gcc $(OOPTS) sll.c

queue.o : queue.c queue.h sll.o
		gcc $(OOPTS) queue.c

bst.o : bst.c bst.h
		gcc $(OOPTS) bst.c

prim.o :    prim.c integer.o real.o string.o scanner.o
		gcc $(OOPTS) prim.c

gst.o : gst.c gst.h bst.o
		gcc $(OOPTS) gst.c

avl.o : avl.c avl.h bst.o
		gcc $(OOPTS) avl.c

stack.o : stack.c stack.h dll.o
		gcc $(OOPTS) stack.c

dll.o : dll.c dll.h
		gcc $(OOPTS) dll.c

test : prim
		$(EXEC)

valgrind : prim
		valgrind $(EXEC)

clean	:
		rm -f -r $(OBJS) prim *.dSYM

memcheck : prim
		valgrind --leak-check=yes $(EXEC)

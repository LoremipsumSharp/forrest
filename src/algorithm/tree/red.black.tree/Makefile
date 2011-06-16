################################
# Makefile for tree algorithms #
################################

# Programs, flags, etc.
CC	= gcc
CFLAGS	= --std=gnu99 -g -c -Wall -I../../../../include/
LFLAGS	= -lm
RM	= /bin/rm

# This Program
BINS	= rbtree
OBJS	= rbtlib.o bst.o CLRS.o Sedgewick.o red.black.tree.o

# All Phony Targets
.PHONY : everything all final clean realclean

# Default starting position
everything : $(BINS)

all : realclean $(BINS)

final : everything clean

clean :
	$(RM) -f $(OBJS)

realclean : clean
	$(RM) -f $(BINS)

# objs
rbtlib.o : rbtlib.c rbtlib.h
	$(CC) $(CFLAGS) -o $@ $<

bst.o : bst.c rbtlib.h
	$(CC) $(CFLAGS) -o $@ $<

CLRS.o : CLRS.c rbtlib.h
	$(CC) $(CFLAGS) -o $@ $<

Sedgewick.o : Sedgewick.c rbtlib.h
	$(CC) $(CFLAGS) -o $@ $<

red.black.tree.o : red.black.tree.c rbtlib.h
	$(CC) $(CFLAGS) -o $@ $<

# bins
rbtree : bst.o CLRS.o Sedgewick.o rbtlib.o red.black.tree.o
	$(CC) -o $@ $^

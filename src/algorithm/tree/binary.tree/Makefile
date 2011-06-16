################################
# Makefile for tree algorithms #
################################

# Programs, flags, etc.
CC	= gcc
CFLAGS	= --std=gnu99 -g -c -Wall -I../../../../include/
LFLAGS	= -lm
RM	= /bin/rm

# This Program
BINS	= preorder2tree inorder binary.search.tree
OBJS	= preorder2tree.o bintreelib.o inorder.o binary.search.tree.o

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
bintreelib.o : bintreelib.c bintreelib.h
	$(CC) $(CFLAGS) -o $@ $<

preorder2tree.o : preorder2tree.c
	$(CC) $(CFLAGS) -o $@ $<

inorder.o : inorder.c bintreelib.h
	$(CC) $(CFLAGS) -o $@ $<

binary.search.tree.o : binary.search.tree.c bintreelib.h
	$(CC) $(CFLAGS) -o $@ $<

# bins
preorder2tree : preorder2tree.o
	$(CC) -o $@ $^

inorder : inorder.o bintreelib.o
	$(CC) -o $@ $^

binary.search.tree : binary.search.tree.o bintreelib.o
	$(CC) -o $@ $^

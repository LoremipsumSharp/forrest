###################################
# Makefile for sorting algorithms #
###################################

# Programs, flags, etc.
CC	= gcc
CFLAGS	= --std=gnu99 -g -c -Wall -I ../../../include/
LFLAGS	= -lm
RM	= /bin/rm

# This Program
BINS	= insertion_sort merge_sort bubble_sort heapsort quicksort stooge_sort counting_sort radix_sort
OBJS	= sortlib.o insertion.sort.o merge.sort.o bubble.sort.o heapsort.o quicksort.o stooge.sort.o counting.sort.o radix.sort.o

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
sortlib.o : sortlib.c sortlib.h
	$(CC) $(CFLAGS) -o $@ $<

insertion.sort.o : insertion.sort.c sortlib.h
	$(CC) $(CFLAGS) -o $@ $<

merge.sort.o : merge.sort.c sortlib.h
	$(CC) $(CFLAGS) -o $@ $<

bubble.sort.o : bubble.sort.c sortlib.h
	$(CC) $(CFLAGS) -o $@ $<

heapsort.o : heapsort.c sortlib.h
	$(CC) $(CFLAGS) -o $@ $<

quicksort.o : quicksort.c sortlib.h
	$(CC) $(CFLAGS) -o $@ $<

stooge.sort.o : stooge.sort.c sortlib.h
	$(CC) $(CFLAGS) -o $@ $<

counting.sort.o : counting.sort.c sortlib.h
	$(CC) $(CFLAGS) -o $@ $<

radix.sort.o : radix.sort.c sortlib.h
	$(CC) $(CFLAGS) -o $@ $<

# bins
insertion_sort : insertion.sort.o sortlib.o
	$(CC) -o $@ $^

merge_sort : merge.sort.o sortlib.o
	$(CC) -o $@ $^

bubble_sort : bubble.sort.o sortlib.o
	$(CC) -o $@ $^

heapsort : heapsort.o sortlib.o
	$(CC) -lm -o $@ $^

quicksort : quicksort.o sortlib.o
	$(CC) -lm -o $@ $^

stooge_sort : stooge.sort.o sortlib.o
	$(CC) -o $@ $^

counting_sort : counting.sort.o sortlib.o
	$(CC) -o $@ $^

radix_sort : radix.sort.o sortlib.o
	$(CC) -o $@ $^

/*
 * Heap Sort
 *
 * author: Forrest Y. Yu <forrest.yu@gmail.com>, http://forrestyu.net/
 */

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <limits.h>
#include <assert.h>

#include "sortlib.h"

#define PARENT(x)	((x - 1) >> 1)
#define LEFT(x)		((x << 1) + 1)
#define RIGHT(x)	((x << 1) + 2)

#define	______________________________	heap2fig

swap_func swap = xor_swap;

/******************************************************************************
 * auxiliary routines for drawing
 ******************************************************************************/
/* calculate coordinates of A[i] */
void calc_coor(int i, double * p, double * q)
{
	const double topx = 20.0;
	const double topy = 20.0;
	double deltax = 4.0;
	double deltay = 1.2;

	int depth = (int)log2(i+1); /* downwards from top */

	double leftmost_x = topx;
	for (int j = depth; j > 0; j--) {
		deltax /= 2.0;
		leftmost_x -= deltax;
	}

	int leftmost = (int)pow(2, depth) - 1;
	/* printf("[%d] depth:%d, leftmost_x:%f, leftmost:%d\n", */
	/*        i, depth, leftmost_x, leftmost); */
	*p = leftmost_x;
	for (int j = 0; j < i - leftmost; j++)
		*p += deltax * 2;

	*q = topy - depth * deltay;
}

/* draw the heap (binary tree) */
void heap2fig(int * A, int Asize, int heapsize, char * label_fmt, ...)
{
	va_list args;
	va_start(args, label_fmt);

	char label[DOT_LABEL_STR_LEN];
	vsnprintf(label, DOT_LABEL_STR_LEN, label_fmt, args);

	const char prefix[] = "heap/heap illustration";
	/* const char HEAP_ATTR[] = ", style=\"filled\", fillcolor=\"green\""; */
	const char HEAP_ATTR[] = "";
	const char SORTED_ATTR[] = ", style=\"filled\", fillcolor=\"gray90\"";
	const char L_ATTR[] = "color=\"gray75\"";
	const char R_ATTR[] = "color=\"gray75\"";

	static int id = 0;

	char all_elements[MAX_NUM_NR * MAX_NUM_WIDTH];
	char * p = all_elements;
	for (int i = 0; i < Asize; i++) {
		p += snprintf(p, MAX_NUM_NR * MAX_NUM_WIDTH, "%s%d%s",
			      i == 0 ? "[" : ", ",
			      A[i],
			      i == Asize - 1 ? "]" : "");
	}
	write_dot(prefix, id, "label = \"heap\\n%s\\n%s\";\n",
		  all_elements, label);

	for (int i = 0; i < Asize; i++) {
		double x;
		double y;
		calc_coor(i, &x, &y);
		write_dot(prefix, id,
			  "\"%X\" [label=\"%d\"%s, pos=\"%.5f,%.5f!\"];\n",
			  &A[i], A[i], i < heapsize ? HEAP_ATTR : SORTED_ATTR, x, y);
		write_dot(prefix, id,
			  "\"%Xidx\" [shape=\"none\", label=\"%d\""
			  ", fontcolor=\"gold\", pos=\"%.5f,%.5f!\"];\n",
			  &A[i], i, x, y+0.4);
		if (LEFT(i) < heapsize)
			write_dot(prefix, id,
				  "\"%X\" -- \"%X\" [%s];\n",
				  (unsigned int)&A[i], (unsigned int)&A[LEFT(i)],
				  L_ATTR);
		if (RIGHT(i) < heapsize)
			write_dot(prefix, id,
				  "\"%X\" -- \"%X\" [%s];\n",
				  (unsigned int)&A[i], (unsigned int)&A[RIGHT(i)],
				  R_ATTR);
	}

	write_dot(prefix, id, "");
	id++;
	va_end(args);
}

/******************************************************************************
 * MaxHeapify, BuildMaxHeap, HeapSort
 ******************************************************************************/
void max_heapify(int * A, int Asize, int heapsize, int i)
{
	static int depth = 0;
	for (int k = 0; k <= depth; k++)
		printf("\t");
	printf("A[%d]: %d\n", i, A[i]);
	depth++;

	int max;
	int l = LEFT(i);
	int r = RIGHT(i);

	if (l < heapsize && A[l] > A[i])
		max = l;
	else
		max = i;

	if (r < heapsize && A[r] > A[max])
		max = r;

	if (max != i) {
		swap(&A[max], &A[i]);
		/* ______________________________(A, Asize, heapsize, "after MAX-HEAPIFY::swap\\n%2d<->%2d\\nA[%2d]<->A[%2d]", A[max], A[i], max, i); */
		max_heapify(A, Asize, heapsize, max);
	}

	depth--;
}

void build_max_heap(int * A, int Asize)
{
	for (int i = (Asize - 1) / 2; i >= 0; i--)
		max_heapify(A, Asize, Asize, i);
}

void heap_sort(int * A, int Asize)
{
	int heap_size = Asize;
	______________________________(A, Asize, heap_size, "=====\\nbegin\\n=====");
	build_max_heap(A, Asize);
	while (heap_size > 1) {
		swap(&A[0], &A[heap_size - 1]);
		heap_size--;
		______________________________(A, Asize, heap_size, "HEAP-SORTing\\n%2d<->%2d\\nA[%2d]<->A[%2d]", A[0], A[heap_size - 1], 0, heap_size - 1);
		max_heapify(A, Asize, heap_size, 0);
	}
	______________________________(A, Asize, 0, "===\\nend\\n===");
}

/******************************************************************************
 * HeapMaximum, HeapExtractMax, HeapIncreaseKey, MaxHeapInsert
 ******************************************************************************/
int heap_maximum(int * A, int heapsize)
{
	assert(heapsize > 0);
	return A[0];
}

int heap_extract_max(int * A, int * heapsize)
{
	assert(*heapsize > 0);
	int max = A[0];
	A[0] = A[--(*heapsize)];
	max_heapify(A, *heapsize, *heapsize, 0);
	return max;
}

void heap_increase_key(int * A, int heapsize, int i, int key)
{
	assert(i < heapsize);
	assert(key > A[i]);
	A[i] = key;
	for (; A[i] > A[PARENT(i)]; i = PARENT(i))
		swap(&A[i], &A[PARENT(i)]);
}

void max_heap_insert(int * A, int Asize, int * heapsize, int key)
{
	assert(*heapsize < Asize);
	A[(*heapsize)++] = INT_MIN;
	heap_increase_key(A, *heapsize, *heapsize - 1, key);
}

/******************************************************************************
 * MAIN
 ******************************************************************************/
int main(int argc, char * argv[])
{
	int A[MAX_NUM_NR];
	char * filename_unsorted = get_filename(argc, argv);
	char * filename_sorted = get_sorted_filename(filename_unsorted);

	int n = get_A(A, MAX_NUM_NR, filename_unsorted);

	print_A(A, n, "original:");
	heap_sort(A, n);
	print_A(A, n, "sorted:");

	assert(verify_A(A, n, filename_sorted));

	/* priority queue */
	int heapsize = n;
	build_max_heap(A, heapsize);
	______________________________(A, n, heapsize, "===============\\nafter build_max_heap\\n===============");

	printf("heap_maximum: %d\n", heap_maximum(A, heapsize));

	printf("heap_extract_max: %d\n", heap_extract_max(A, &heapsize));
	print_A(A, heapsize, "after heap_extract_max");
	______________________________(A, n, heapsize, "===============\\nafter heap_extract_max\\n===============");

	heap_increase_key(A, heapsize, 3, 88);
	print_A(A, heapsize, "after heap_increase_key");
	______________________________(A, n, heapsize, "===============\\nafter heap_increase_key\\n===============");

	max_heap_insert(A, n, &heapsize, 97);
	print_A(A, heapsize, "after max_heap_insert");
	______________________________(A, n, heapsize, "===============\\nafter max_heap_insert\\n===============");

	return 0;
}

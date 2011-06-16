/*
 * Bubble Sort
 *
 * author: Forrest Y. Yu <forrest.yu@gmail.com>, http://forrestyu.net/
 */

#include <stdio.h>
#include <assert.h>

#include "sortlib.h"

void bubble_sort(int * A, int lim)
{
	for (int i = 0; i < lim; i++)
		for (int j = lim - 1; j > i; j--)
			if (A[j - 1] > A[j])
				xor_swap(&A[j - 1], &A[j]);
}

int main(int argc, char * argv[])
{
	int A[MAX_NUM_NR];
	char * filename_unsorted = get_filename(argc, argv);
	char * filename_sorted = get_sorted_filename(filename_unsorted);

	int n = get_A(A, MAX_NUM_NR, filename_unsorted);

	print_A(A, n, "original:");
	bubble_sort(A, n);
	print_A(A, n, "sorted:");

	assert(verify_A(A, n, filename_sorted));

	return 0;
}

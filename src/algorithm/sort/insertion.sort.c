/*
 * Insertion Sort
 *
 * author: Forrest Y. Yu <forrest.yu@gmail.com>, http://forrestyu.net/
 */

#include <stdio.h>
#include <assert.h>

#include "sortlib.h"

/******************************************************************************
 * Insertion Sort
 ******************************************************************************/
void insertion_sort(int * A, int lim)
{
	int i;
	int j;
	for (i = 1; i < lim; i++) {
		int key = A[i];
		for (j = i - 1; j >= 0; j--) {
			if (A[j] <= key)
				break;
			A[j+1] = A[j];
		}
		A[j+1] = key;
	}
}

/******************************************************************************
 * main
 ******************************************************************************/
int main(int argc, char * argv[])
{
	int A[MAX_NUM_NR];
	char * filename_unsorted = get_filename(argc, argv);
	char * filename_sorted = get_sorted_filename(filename_unsorted);

	int n = get_A(A, MAX_NUM_NR, filename_unsorted);

	print_A(A, n, "original:");
	insertion_sort(A, n);
	print_A(A, n, "sorted:");

	assert(verify_A(A, n, filename_sorted));

	return 0;
}

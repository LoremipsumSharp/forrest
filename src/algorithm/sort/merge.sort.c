/*
 * Merge Sort
 *
 * author: Forrest Y. Yu <forrest.yu@gmail.com>, http://forrestyu.net/
 */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#include "sortlib.h"

/******************************************************************************
 * Merge Sort
 ******************************************************************************/
void normal_merge(int * R, int * A, int lim_a, int * B, int lim_b)
{
        int * a = A;
        int * b = B;
        int k = 0;
        while (a < &A[lim_a] && b < &B[lim_b]) {
                if (*a < *b)
                        R[k++] = *a++;
                else
                        R[k++] = *b++;
        }
        while (a < &A[lim_a])
                R[k++] = *a++;
        while (b < &B[lim_b])
                R[k++] = *b++;
}

void sentinel_merge(int * R, int * A, int lim_a, int * B, int lim_b)
{
	assert((long long int)LLONG_MAX > (long long int)INT_MAX);
	long long int infty = LLONG_MAX;

	int * a = A;
	int * b = B;
	int k = 0;
	while (a < &A[lim_a] || b < &B[lim_b]) {
		long long int x = a < &A[lim_a] ? *a : infty;
		long long int y = b < &B[lim_b] ? *b : infty;
		if (x < y) {
			R[k++] = (int)x;
			a++;
		} else {
			R[k++] = (int)y;
			b++;
		}
	}
}

enum merge_mode {normal, sentinel};
void merge_sort(int * A, int lim, enum merge_mode mode)
{
	int mid;
	int r[MAX_NUM_NR];

	if (lim <= 1)
		return;

	mid = lim / 2;
	merge_sort(A, mid, mode);
	merge_sort(A + mid, lim - mid, mode);
	if (mode == sentinel)
		sentinel_merge(r, A, mid, A + mid, lim - mid);
	else
		normal_merge(r, A, mid, A + mid, lim - mid);
	memcpy(A, r, sizeof(A[0]) * lim);
}

/******************************************************************************
 * main
 ******************************************************************************/
int main(int argc, char * argv[])
{
	int n;
	int A[MAX_NUM_NR];
	char * filename_unsorted = get_filename(argc, argv);
	char * filename_sorted = get_sorted_filename(filename_unsorted);

	int i;
	for (i = 0; i < 2; i++) {
		n = get_A(A, MAX_NUM_NR, filename_unsorted);
		print_A(A, n, "original:");
		merge_sort(A, n, i == 0 ? normal : sentinel);
		print_A(A, n, "sorted (merge sort, %s merge):",
			i == 0 ? "normal" : "sentinel");
		assert(verify_A(A, n, filename_sorted));
	}

	return 0;
}

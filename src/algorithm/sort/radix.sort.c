/*
 * Radix Sort
 *
 * author: Forrest Y. Yu <forrest.yu@gmail.com>, http://forrestyu.net/
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <getopt.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>

#include "colorful.h"
#include "sortlib.h"

int verbose = 0;

#define	___________________________________	 print_A
#define	___________________________________1	 if (verbose >= 1) print_A
#define	___________________________________2	 if (verbose >= 2) print_A

/******************************************************************************
 * usage
 ******************************************************************************/
void print_usage(const char * bin_name)
{
	printf("%s", GREEN_B);
	printf("Usage:\n");
	printf("\t%s -f test_filename -r radix [-v 0|1|2]\n", bin_name);
	printf("\t%s --file test_filename --radix radix [--verbose 0|1|2]\n",
	       bin_name);
	printf("%s", NOCOLOR);
}

/******************************************************************************
 * Counting Sort
 ******************************************************************************/
int key(int x, int r, int idx)
{
	int mask = (1 << (r * (idx + 1))) - 1;
	int k = (x & mask) >> (r * idx);
	if (verbose >= 2)
		printf("mask: %#X, key: %#X\n", mask, k);
	return k;
}

void counting_sort(int * A, int n, int r, int idx)
{
	assert(n);
	assert(r);

	int k = 2 << r;
	int m = k + 1;		/* 0..k */

	int * B = (int*)malloc(sizeof(int) * n);
	assert(B);
	int * C = (int*)malloc(sizeof(int) * m);
	assert(C);

	for (int i = 0; i < m; i++)
		C[i] = 0;
	___________________________________2(C, m, "C[0..%d]:", k);

	for (int j = 0; j < n; j++) {
		int A_key = key(A[j], r, idx);
		assert(A_key < m);
		C[A_key]++;
	}
	___________________________________2(C, m, "C[0..%d]:", k);

	for (int i = 1; i < m; i++)
		C[i] += C[i-1];
	___________________________________2(C, m, "C[0..%d]:", k);

	for (int j = n - 1; j >= 0; j--)
		B[(C[key(A[j], r, idx)]--)-1] = A[j];
	___________________________________2(B, n, "B[0..%d]:", n);

	memcpy(A, B, sizeof(int) * n);

	free(C);
	free(B);
}

/******************************************************************************
 * Radix Sort
 ******************************************************************************/
void radix_sort(int * A, int n, int b, int r)
{
	/*
	 * |---------------------------b---------------------------|
	 * |------r------|
	 *
	 * +-------------+-------------+-------------+-------------+
	 * | digit b/r-1 |             |   digit 1   |   digit 0   |
	 * +-------------+-------------+-------------+-------------+
	 *
	 * |----------------------b/r digits-----------------------|
	 */
	assert(n);
	assert(b);
	assert(r > 0 && r <= 12); /* too big radix is trouble */

	int digit_nr = b / r;
	for (int i = 0; i < digit_nr; i++) {
		___________________________________1(A, n, "0 - A[0..%d]:", n);
		counting_sort(A, n, r, i);
		___________________________________1(A, n, "1 - A[0..%d]:", n);
	}
}

int main(int argc, char * argv[])
{
	int A[MAX_NUM_NR];
	char * filename_unsorted = 0;
	int r = 0;

	opterr = 0;
	while (1) {
		int c;
		static struct option long_options[] = {
			{"verbose",      required_argument, 0, 'v'},
			{"radix",        required_argument, 0, 'r'},
			{"file",         required_argument, 0, 'f'},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;
     
		c = getopt_long(argc, argv, "v:r:f:",
				long_options, &option_index);
     
		/* Detect the end of the options. */
		if (c == -1)
			break;
     
		switch (c) {
		case 0:
			/* If this option set a flag, do nothing else now. */
			if(long_options[option_index].flag != 0)
				break;
			printf("option %s", long_options[option_index].name);
			if(optarg)
				printf(" with arg %s", optarg);
			printf("\n");
			break;
     
		case 'v':
			verbose = atoi_or_abort(optarg);
			printf("%sverbose:%d%s\n", GREEN_B, verbose, NOCOLOR);
			break;
     
		case 'r':
			r = atoi_or_abort(optarg);
			printf("%sr:%d%s\n", GREEN_B, r, NOCOLOR);
			break;
     
		case 'f':
			filename_unsorted = optarg;
			break;
     
		case '?':
			/* getopt_long already printed an error message. */
			printf("%sbad arg. %s%s\n", RED_B, argv[optind-1], NOCOLOR);
			break;
     
		default:
			abort();
		}
	}
	/* Print any remaining command line arguments (not options). */
	if (optind < argc)
	{
		printf("these ARGV-elements will be ignored: %s", RED);
		while (optind < argc)
			printf ("%s ", argv[optind++]);
		printf("%s\n", NOCOLOR);
	}

	if (!filename_unsorted) {
		printf("%sNo test case file given, abort.%s\n", RED_B, NOCOLOR);
		print_usage(argv[0]);
		abort();
	}

	if (!r) {
		printf("%sNo radix is given, abort.%s\n", RED_B, NOCOLOR);
		print_usage(argv[0]);
		abort();
	}

	char * filename_sorted = get_sorted_filename(filename_unsorted);

	int n = get_A(A, MAX_NUM_NR, filename_unsorted);

	print_A(A, n, "original:");
	radix_sort(A, n, 32, r);
	/* counting_sort(A, n, r, 0); */
	print_A(A, n, "sorted:");

	assert(verify_A(A, n, filename_sorted));

	return 0;
}

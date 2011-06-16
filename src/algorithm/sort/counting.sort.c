/*
 * Counting Sort
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

#define	___________________________________	 print_A

/******************************************************************************
 * usage
 ******************************************************************************/
void print_usage(const char * bin_name)
{
	printf("%s", GREEN_B);
	printf("Usage:\n");
	printf("\t%s -f test_filename -k max_num\n", bin_name);
	printf("\t%s --file test_filename --max-num max_num\n", bin_name);
	printf("%s", NOCOLOR);
}

/******************************************************************************
 * Counting Sort
 ******************************************************************************/
void counting_sort(int * A, int n, int k)
{
	assert(n);
	assert(k);

	int m = k + 1;		/* 0..k */

	int * B = (int*)malloc(sizeof(int) * n);
	assert(B);
	int * C = (int*)malloc(sizeof(int) * m);
	assert(C);

	for (int i = 0; i < m; i++)
		C[i] = 0;
	___________________________________(C, m, "C[0..%d]:", k);

	for (int j = 0; j < n; j++) {
		assert(A[j] < m);
		C[A[j]]++;
	}
	___________________________________(C, m, "C[0..%d]:", k);

	for (int i = 1; i < m; i++)
		C[i] += C[i-1];
	___________________________________(C, m, "C[0..%d]:", k);

	for (int j = n - 1; j >= 0; j--)
		B[(C[A[j]]--)-1] = A[j];
	___________________________________(B, n, "B[0..%d]:", n);

	memcpy(A, B, sizeof(int) * n);

	free(C);
	free(B);
}

int main(int argc, char * argv[])
{
	int A[MAX_NUM_NR];
	char * filename_unsorted = 0;
	int k = 0;

	opterr = 0;
	while (1) {
		int c;
		static struct option long_options[] = {
			{"max-num",      required_argument, 0, 'k'},
			{"file",         required_argument, 0, 'f'},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;
     
		c = getopt_long(argc, argv, "k:f:",
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
     
		case 'k':
			k = atoi_or_abort(optarg);
			printf("%sk:%d%s\n", GREEN_B, k, NOCOLOR);
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

	if (!k) {
		printf("%sNo `k' is given, abort.%s\n", RED_B, NOCOLOR);
		print_usage(argv[0]);
		abort();
	}

	char * filename_sorted = get_sorted_filename(filename_unsorted);

	int n = get_A(A, MAX_NUM_NR, filename_unsorted);

	print_A(A, n, "original:");
	counting_sort(A, n, k);
	print_A(A, n, "sorted:");

	assert(verify_A(A, n, filename_sorted));

	return 0;
}

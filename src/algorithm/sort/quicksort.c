/*
 * Quicksort
 *
 * author: Forrest Y. Yu <forrest.yu@gmail.com>, http://forrestyu.net/
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <getopt.h>
#include <assert.h>

#include "colorful.h"
#include "sortlib.h"

static int fig_flag = 0;
static int depth = 0;

#define	______________________________	sorting_fig

#define TRACE_STACK_BEGIN	do					\
	{								\
		++depth;						\
		printf("%s", YELLOW);					\
		/* printf("stack depth:%5d ", depth); */		\
		printf("(%3d ~ %3d) ", p, r);				\
		for (int _i_ = 0; _i_ < Asize; _i_++)			\
			printf("%c", (_i_ < p || _i_ > r) ? '-' : '*');	\
		for (int _i_ = 0; _i_ < depth; _i_++)			\
			printf("|");					\
		printf("%s\n", NOCOLOR);				\
	} while (0);
#define TRACE_STACK_END		--depth;

swap_func swap = xor_swap;

/******************************************************************************
 * auxiliary routines for drawing
 ******************************************************************************/
void sorting_fig(int * A, int Asize, int p, int i, int j, int r, int q, char * label_fmt, ...)
{
	if (!fig_flag)
		return;

	va_list args;
	va_start(args, label_fmt);

	char label[DOT_LABEL_STR_LEN];
	vsnprintf(label, DOT_LABEL_STR_LEN, label_fmt, args);

	const char prefix[] = "quicksort/qsort illustration";
	const char NUM_ATTR[] = "shape=\"none\", fontsize=\"8\"";
	const char VAR_ATTR[] = "shape=\"none\", fixedsize=\"false\", fontcolor=\"magenta\"";
	const char LINE_ATTR[] = "penwidth=\"5\", color=\"gray\"";
	const char HIGHLIGHT_LINE_ATTR[] = "penwidth=\"5\", color=\"red\"";
	/* const char BASELINE_ATTR[] = "color=\"gray\""; */
	const char RANGE_ATTR[] = "color=\"yellowgreen\"";

	static int id = 0;

	char all_elements[MAX_NUM_NR * MAX_NUM_WIDTH];
	char * _p = all_elements;
	for (int m = 0; m < Asize; m++) {
		if (m > 0 && m % 20 == 0)
			_p += snprintf(_p, MAX_NUM_NR * MAX_NUM_WIDTH, "\\n");
		else
			_p += snprintf(_p, MAX_NUM_NR * MAX_NUM_WIDTH, "%s%d%s",
				       m == 0 ? "[" : "",
				       A[m],
				       m == Asize - 1 ? "]" : ", ");
	}
	write_dot(prefix, id, "label = \"quicksort\\n%s\\n%s\";\n",
		  all_elements, label);

	enum {VAR_NR=5, STR_LEN=16};
	char * var_names[VAR_NR] = {"p", "i", "j", "r", "⇑"/* "↑⇑⇧☝⤒⬆" */};
	int vars[VAR_NR];
	vars[0] = p; vars[1] = i; vars[2] = j; vars[3] = r; vars[4] = q;
	char vars_str[STR_LEN];

	double xratio;
	double yratio;
	if (Asize < 10) {
		xratio = 1.0;
		yratio = 0.5;
	} else if (Asize < 16) {
		xratio = 0.5;
		yratio = 0.2;
	} else if (Asize < 30) {
		xratio = 0.3;
		yratio = 0.03;
	} else {
		xratio = 0.2;
		yratio = 0.03;
	}
	double ydelta = 0.5;	/* to compensate for the node height */
	for (int m = -1; m <= Asize; m++) {
		double x = (double)m * xratio;
		double y = (double)(A[m]) * yratio + ydelta;
		if (m < 0 || m >= Asize) {
			write_dot(prefix, id,
				  "\"reserved_%d\" [label=\"\", shape=\"none\""
				  ", style=\"invis\", fixedsize=true, width=0.0"
				  ", pos=\"%.5f,%.5f!\"];\n",
				  m, x < 0 ? (x - 0.5) * xratio : (x + 0.5) * xratio, -1.0);
		} else {
			write_dot(prefix, id,
				  "\"%X\" [label=\"%d\", %s, pos=\"%.5f,%.5f!\"];\n",
				  &A[m], A[m], NUM_ATTR, x, y);
			write_dot(prefix, id,
				  "\"%X_base\" [label=\"\", %s, pos=\"%.5f,%.5f!\"];\n",
				  &A[m], /* A[m], */ NUM_ATTR, x, 0.0);
			write_dot(prefix, id,
				  "\"%X\" -- \"%X_base\" [%s];\n",
				  &A[m], &A[m], m == q ? HIGHLIGHT_LINE_ATTR : LINE_ATTR);
		}
		char * vs = vars_str;
		*vs = 0;
		for (int t = 0; t < VAR_NR; t++) {
			if (m < Asize && m == vars[t])
				vs += snprintf(vs, STR_LEN, "%s%s",
					       vs == vars_str ? "" : "\\n",
					       var_names[t]);
		}
		write_dot(prefix, id,
			  "\"vars%d\" [label=\"%s\", %s, pos=\"%.5f,%.5f!\"];\n",
			  m, vars_str, VAR_ATTR, x, -0.5);
	}
	/* write_dot(prefix, id, */
	/* 	  "\"reserved_%d\" -- \"reserved_%d\" [%s];\n", */
	/* 	  -1, Asize, BASELINE_ATTR); */

	if (p < r) {
		write_dot(prefix, id,
			  "\"current_range_0\" [label=\"\", shape=\"point\""
			  ", color=\"yellowgreen\", fixedsize=true, width=0.1"
			  ", pos=\"%.5f,%.5f!\"];\n",
			  (double)p * xratio, -1.0);
		write_dot(prefix, id,
			  "\"current_range_1\" [label=\"\", shape=\"point\""
			  ", color=\"yellowgreen\", fixedsize=true, width=0.1"
			  ", pos=\"%.5f,%.5f!\"];\n",
			  (double)r * xratio, -1.0);
		write_dot(prefix, id,
			  "\"current_range_0\" -- \"current_range_1\" [%s];\n",
			  RANGE_ATTR);
	}

	write_dot(prefix, id, "");
	id++;
	va_end(args);
}

/******************************************************************************
 * Quicksort
 ******************************************************************************/
int partition(int * A, int Asize, int p, int r)
{
	int i = p - 1;
	int x = A[r];
	for (int j = p; j <= r - 1; j++) {
		______________________________(A, Asize, p, i, j, r, Asize, "partition(A, %d, %d) : 0", p, r);
		if (A[j] < x) {
			i++;
			swap(&A[i], &A[j]);
			______________________________(A, Asize, p, i, j, r, Asize, "partition(A, %d, %d) : 1", p, r);
		}
	}
	swap(&A[i+1], &A[r]);
	______________________________(A, Asize, p, i, Asize, r, i+1, "partition(A, %d, %d) : done.", p, r);

	return i+1;
}

void quicksort(int * A, int Asize, int p, int r)
{
	TRACE_STACK_BEGIN;
	if (p < r) {
		int q = partition(A, Asize, p, r);
		quicksort(A, Asize, p, q - 1);
		quicksort(A, Asize, q + 1, r);
	}
	TRACE_STACK_END;
}

/******************************************************************************
 * Quicksort (randomized partitioning)
 ******************************************************************************/
int randomized_partition(int * A, int Asize, int p, int r)
{
	int k = rand_in_range_inclusive(p, r);
	swap(&A[k], &A[r]);
	______________________________(A, Asize, p, Asize, Asize, r, Asize, "swapped randomly chosen A[%d] and A[%d])", k, r);
	return partition(A, Asize, p, r);
}

void randomized_quicksort(int * A, int Asize, int p, int r)
{
	if (p < r) {
		int q = randomized_partition(A, Asize, p, r);
		randomized_quicksort(A, Asize, p, q - 1);
		randomized_quicksort(A, Asize, q + 1, r);
	}
}

/******************************************************************************
 * Quicksort (original version by C. A. R. Hoare)
 ******************************************************************************/
#define repeat		do
#define until		while (!(
#define _repeat		))
int Hoare_partition(int * A, int Asize, int p, int r)
{
	int x = A[p];
	int i = p - 1;
	int j = r + 1;
	while (1) {
		repeat
			j--;
		until A[j] <= x _repeat;
		______________________________(A, Asize, p, i, j, r, Asize, "Hoare's partition : A[j] ≤ x (%d)", x);

		repeat
			i++;
		until A[i] >= x _repeat;
		______________________________(A, Asize, p, i, j, r, Asize, "Hoare's partition : A[i] ≥ x (%d)", x);

		if (i < j) {
			swap(&A[i], &A[j]);
			______________________________(A, Asize, p, i, j, r, Asize, "Hoare's partition : swapped A[%d] and A[%d]", i, j);
		} else {
			break;
		}
	}
	______________________________(A, Asize, p, i, j, r, Asize, "Hoare's partition : return j (%d)", j);
	return j;
}

void Hoare_quicksort(int * A, int Asize, int p, int r)
{
	if (p < r) {
		int q = Hoare_partition(A, Asize, p, r);
		Hoare_quicksort(A, Asize, p, q);
		Hoare_quicksort(A, Asize, q + 1, r);
	}
}

/******************************************************************************
 * tail recursion Quicksort
 ******************************************************************************/
void tail_recursion_quicksort(int * A, int Asize, int p, int r)
{
	TRACE_STACK_BEGIN;
	while (p < r) {
		int q = partition(A, Asize, p, r);
		tail_recursion_quicksort(A, Asize, p, q - 1);
		p = q + 1;
	}
	TRACE_STACK_END;
}

/******************************************************************************
 * Quicksort with worst-case stack depth is 𝚯(lg(n))
 ******************************************************************************/
void shallow_stack_quicksort(int * A, int Asize, int p, int r)
{
	TRACE_STACK_BEGIN;
	while (p < r) {
		int q = partition(A, Asize, p, r);
		if (q - p < r - q) {
			tail_recursion_quicksort(A, Asize, p, q - 1);
			p = q + 1;
		} else {
			tail_recursion_quicksort(A, Asize, q + 1, r);
			r = q - 1;
		}
	}
	TRACE_STACK_END;
}

int main(int argc, char * argv[])
{
	int A[MAX_NUM_NR];
	char * filename_unsorted = 0;

	void (*qsort)(int* , int, int, int) = quicksort;

	opterr = 0;
	while (1) {
		int c;
		static struct option long_options[] = {
			/* These options set a flag. */
			{"fig",            no_argument,       &fig_flag, 1},
			{"nofig",          no_argument,       &fig_flag, 0},
			/* These options don't set a flag.
			   We distinguish them by their indices. */
			{"randomized",     no_argument,       0, 'r'},
			{"Hoare",          no_argument,       0, 'H'},
			{"tail-recursion", no_argument,       0, 't'},
			{"shallow-stack",  no_argument,       0, 's'},
			{"file",           required_argument, 0, 'f'},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;
     
		c = getopt_long(argc, argv, "rHtsf:",
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
     
		case 'r':
			printf("%srandomized quicksort%s\n", GREEN_B, NOCOLOR);
			qsort = randomized_quicksort;
			break;
     
		case 'H':
			printf("%sHoare's quicksort%s\n", GREEN_B, NOCOLOR);
			qsort = Hoare_quicksort;
			break;
     
		case 't':
			printf("%stail recursion quicksort%s\n", GREEN_B, NOCOLOR);
			qsort = tail_recursion_quicksort;
			break;
     
		case 's':
			printf("%squicksort with worst-case stack depth is 𝚯(lg(n))%s\n", GREEN_B, NOCOLOR);
			qsort = shallow_stack_quicksort;
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
		abort();
	}

	char * filename_sorted = get_sorted_filename(filename_unsorted);

	int n = get_A(A, MAX_NUM_NR, filename_unsorted);

	print_A(A, n, "original:");
	______________________________(A, n, n, n, n, n, n, "ORIGIN");
	qsort(A, n, 0, n-1);
	print_A(A, n, "sorted:");
	______________________________(A, n, n, n, n, n, n, "DONE");

	assert(verify_A(A, n, filename_sorted));

	return 0;
}

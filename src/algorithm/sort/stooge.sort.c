/*
 * Stooge Sort
 *
 * author: Forrest Y. Yu <forrest.yu@gmail.com>, http://forrestyu.net/
 */

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include "colorful.h"
#include "sortlib.h"

#define	______________________________	sorting_fig

swap_func swap = xor_swap;

/******************************************************************************
 * auxiliary routines for drawing
 ******************************************************************************/
void sorting_fig(int * A, int Asize, int i, int j, char * label_fmt, ...)
{
	va_list args;
	va_start(args, label_fmt);

	char label[DOT_LABEL_STR_LEN];
	vsnprintf(label, DOT_LABEL_STR_LEN, label_fmt, args);

	const char prefix[] = "stoogesort/stooge-sort illustration";
	const char NUM_ATTR[] = "shape=\"none\", fontsize=\"8\"";
	const char VAR_ATTR[] = "shape=\"none\", fixedsize=\"false\", fontcolor=\"magenta\"";
	const char LINE_ATTR[] = "penwidth=\"5\", color=\"gray\"";
	/* const char HIGHLIGHT_LINE_ATTR[] = "penwidth=\"5\", color=\"red\""; */
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
	write_dot(prefix, id, "label = \"stooge sort\\n%s\\n%s\";\n",
		  all_elements, label);

	enum {VAR_NR=2, STR_LEN=16};
	char * var_names[VAR_NR] = {"i", "j"};
	int vars[VAR_NR];
	vars[0] = i; vars[1] = j;
	char vars_str[STR_LEN];

	double xratio;
	double yratio;
	if (Asize < 10) {
		xratio = 1.0;
		yratio = 0.3;
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
				  &A[m], &A[m], LINE_ATTR);
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

	if (i < j) {
		write_dot(prefix, id,
			  "\"current_range_0\" [label=\"\", shape=\"point\""
			  ", color=\"yellowgreen\", fixedsize=true, width=0.1"
			  ", pos=\"%.5f,%.5f!\"];\n",
			  (double)i * xratio, -1.0);
		write_dot(prefix, id,
			  "\"current_range_1\" [label=\"\", shape=\"point\""
			  ", color=\"yellowgreen\", fixedsize=true, width=0.1"
			  ", pos=\"%.5f,%.5f!\"];\n",
			  (double)j * xratio, -1.0);
		write_dot(prefix, id,
			  "\"current_range_0\" -- \"current_range_1\" [%s];\n",
			  RANGE_ATTR);
	}

	write_dot(prefix, id, "");
	id++;
	va_end(args);
}

/******************************************************************************
 * Stooge Sort
 ******************************************************************************/
void stooge_sort(int * A, int Asize, int i, int j)
{
	______________________________(A, Asize, i, j, "Stooge-Sort");

	if (A[i] > A[j])
		swap(&A[i], &A[j]);
	if (i + 1 >= j)
		return;
	int k = (j - i + 1) / 3;
	stooge_sort(A, Asize, i, j - k);
	stooge_sort(A, Asize, i + k, j);
	stooge_sort(A, Asize, i, j - k);
}

int main(int argc, char * argv[])
{
	int A[MAX_NUM_NR];
	char * filename_unsorted = get_filename(argc, argv);
	char * filename_sorted = get_sorted_filename(filename_unsorted);

	int n = get_A(A, MAX_NUM_NR, filename_unsorted);

	print_A(A, n, "original:");
	stooge_sort(A, n, 0, n - 1);
	print_A(A, n, "sorted:");

	assert(verify_A(A, n, filename_sorted));

	return 0;
}


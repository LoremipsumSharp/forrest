/*
 * Sort Lib: helper routines for sorting algorithms
 *
 * author: Forrest Y. Yu <forrest.yu@gmail.com>, http://forrestyu.net/
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <errno.h>
#include <err.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "colorful.h"
#include "sortlib.h"

/******************************************************************************
 * define
 ******************************************************************************/
#define	TEST_CASE_FILE		""
//#define	TEST_CASE_FOLDER	"./test.cases"
//#define	TEST_CASE_FILE		TEST_CASE_FOLDER"/10-integers-less-than-100.txt"
//#define	TEST_CASE_FILE		TEST_CASE_FOLDER"/100-integers-less-than-100.txt"
//#define	TEST_CASE_FILE		TEST_CASE_FOLDER"/null"
//#define	TEST_CASE_FILE		TEST_CASE_FOLDER"/nonexist_file"

/******************************************************************************
 * routines
 ******************************************************************************/
void xor_swap(int * p, int * q)
{
	if (p == q)
		return;

	*p ^= *q;	/* a^b, b */
	*q ^= *p;	/* a^b, a */
	*p ^= *q;	/* b,   a */
}

/* returns a random number k such as l ⩽ k ⩽ r */
int rand_in_range_inclusive(int l, int r)
{
	int width = r + 1 - l;
	srand(time(0));
	return (rand() % width) + l;
}

/* @see man strtol */
int atoi_or_abort(const char * str)
{
	int base = 10;
	char *endptr;
	long val;

	errno = 0;    /* To distinguish success/failure after call */
	val = strtol(str, &endptr, base);

	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
	    || (errno != 0 && val == 0)) {
		fprintf(stderr, "%sstrtol failed.%s\n", RED_B, NOCOLOR);
		perror("strtol");
		abort();
	}

	if (endptr == str) {
		fprintf(stderr, "%sInvald argument: %s%s\n",
			RED_B, str, NOCOLOR);
		abort();
	}

	if (*endptr != '\0') {
		fprintf(stderr,
			"%sThere are unexpected characters after number: %s%s\n",
			RED_B, endptr, NOCOLOR);
		abort();
	}

	assert(val < INT_MAX);

	return (int)val;
}

void print_A(int * A, int lim, char * title_fmt, ...)
{
	va_list args;
	va_start(args, title_fmt);

	int i;
	printf("<%s%d%s nums> ", CYAN, lim, NOCOLOR);
	vprintf(title_fmt, args);
	printf("%s\n", MAGENTA);
	for (i = 0; i < lim; i++)
		printf("%d ", A[i]);
	printf("%s\n\n", NOCOLOR);

	va_end(args);
}

char * get_filename(int argc, char * argv[])
{
	char * filename;

	if (sizeof(TEST_CASE_FILE) != 1) {
		filename = TEST_CASE_FILE;
	} else {
		if (argc != 2) {
			fprintf(stderr, "Usage: %s test_case_file\n", argv[0]);
			exit(EXIT_FAILURE);
		}
		filename = argv[1];
	}
	return filename;
}

char * get_sorted_filename(const char * unsorted_filename)
{
	static char sorted_filename[PATH_MAX];
	strcpy(sorted_filename, unsorted_filename);
	if (strcmp(sorted_filename + strlen(sorted_filename) - 7, ".sorted") != 0)
		strcat(sorted_filename, ".sorted");
	return sorted_filename;
}

int get_A(int * A, int lim, char * filename)
{
	FILE * fp = fopen(filename, "r");
	if (!fp)
		F_ERR(fp, "failed to open test case file ``%s%s%s''",
		      ERR_EMPH_COLOR, filename, ERR_COLOR);

	char line[LINE_MAX];
	if (!fgets(line, LINE_MAX, fp)) {
		F_ERR(fp,
		      "failed to read the first line (which is supposed to"
		      " be the comment) from file: %s%s%s",
		      ERR_EMPH_COLOR, filename, NOCOLOR);
	}
	if (line[0] != '#') {
		/* F_ERR(fp, "this line should begin with a `#':\n%s%s%s", */
		/*       ERR_EMPH_COLOR, line, NOCOLOR); */
		fseek(fp, 0, SEEK_SET); /* no comment,
					 * should read from the very beginning
					 */
	}

	int i = 0;
	int num;
	while (fscanf(fp, "%d", &num) == 1) {
		assert(i < lim);
		A[i++] = num;
	}

	fclose(fp);

	return i;
}

int verify_A(int * A, int lim, char * sorted_filename)
{
	FILE * fp = fopen(sorted_filename, "r");
	if (!fp)
		F_ERR(fp, "failed to open sorted file ``%s%s%s''",
		      ERR_EMPH_COLOR, sorted_filename, ERR_COLOR);

	int * p = A;
	int num;
	while (fscanf(fp, "%d", &num) == 1) {
		if (*p != num)
			break;
		p++;
	}

	int v = (p == A + lim && feof(fp)) ? 1 : 0;

	fclose(fp);

	if (v)
		printf("%sGOOD%s (the result is identical to ``%s'')\n",
		       GREEN, NOCOLOR, sorted_filename);
	else
		printf("%sFAIL%s (the result differs from ``%s'')\n",
		       RED, NOCOLOR, sorted_filename);
	printf("\n");

	return v;
}

void mkdir_if_needed(char * path)
{
	char * p;
	fprintf(stderr, "%s", INFO_COLOR);
	for (p = path; *p; p++) {
		if (*p != '/')
			continue;

		*p = 0;		/* change ``path'' temporarily */
		fprintf(stderr, "mkdir %s:\n    ", path);
		int ret = mkdir(path, 0755);
		if (ret) {
			assert(ret == -1);
			if (errno == EEXIST) {
				fprintf(stderr, "folder already exists\n");
			} else {
				fprintf(stderr, "%s", NOCOLOR);
				err(EXIT_FAILURE, "failed to mkdir ``%s''", path);
			}
		} else {
			fprintf(stderr, "done\n");
		}
		*p = '/';	/* restore ``path'' */
	}
	fprintf(stderr, "%s\n", NOCOLOR);
}

/* routine via which the dot file is written */
void write_dot(const char * prefix, int id, const char *fmt, ...)
{
	static int flag = 0;
	static FILE * dotf[MAX_DOTF_NR];
	static char dotfilename[MAX_DOTF_NR][FILENAME_MAX];
	char cmd[CMD_MAX];
	char * fig_folder = "fig";

	va_list args;
	va_start(args, fmt);

	assert(id >= 0 && id < MAX_DOTF_NR);
	if (id < LOWEST_DOT_ID)
		return;

	if (!flag) {
		int i;
		for (i = 0; i < MAX_DOTF_NR; i++)
			dotf[i] = 0;
		flag = 1;
	}
	if (!dotf[id]) {
		sprintf(dotfilename[id], "%s/%s (%03d).%s",
			fig_folder, prefix, id, GRAPHVIZ_FILE_EXT);

		if (flag == 1) {
			flag = 2;
			mkdir_if_needed(dotfilename[id]);
		}
		fprintf(stderr, "%sdot file: %s%s\n", INFO_COLOR,
			dotfilename[id], NOCOLOR);

		dotf[id] = fopen(dotfilename[id], "w");
		if (dotf[id] == 0)
			F_ERR(dotf[id], "failed to open file ``%s%s%s''",
			      ERR_EMPH_COLOR, dotfilename[id], ERR_COLOR);

		fprintf(dotf[id], "graph sort {\n");

		fprintf(dotf[id],
			"\tnode [shape=circle"
			", fontname=Courier New"
			", penwidth=0.5"
			", fixedsize=true"
			", width=0.6"
			/* ", height=0.7" */
			", splines=\"compound\""
			"];\n");
	}

	assert(dotf[id] != 0);

	/* if the dot file becomes big, something must be wrong */
	int dot_f_max_size = 32*1024;
	if (ftell(dotf[id]) > dot_f_max_size) {
		fprintf(stderr,
			"%sdot file too big: %s"
			"%s%s%s"
			"%s, something must be wrong.\n%s",
			YELLOW, NOCOLOR,
			RED, dotfilename[id], NOCOLOR,
			YELLOW, NOCOLOR);
		assert(ftell(dotf[id]) <= dot_f_max_size);
	}

	if (!fmt[0]) {
		fprintf(dotf[id], "}\n");
		fclose(dotf[id]);
		dotf[id] = 0;
		snprintf(cmd, (size_t)CMD_MAX, "%s -T%s \"%s\" -o "
			 "\"%s/%s (%03d).%s\"",
			 GRAPHVIZ_CMD, GRAPHVIZ_FIG_TYPE, dotfilename[id],
			 fig_folder, prefix, id, GRAPHVIZ_FIG_TYPE);
		printf("%s$ %s%s%s\n", WHITE, WHITE, cmd, NOCOLOR);
		/* printf("%s$ %s%s%s\n", WHITE, GREEN, cmd, NOCOLOR); */
		assert(system(cmd) == 0);
	} else {
		fprintf(dotf[id], "\t");
		vfprintf(dotf[id], fmt, args);
	}

	va_end(args);
}


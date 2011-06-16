/*
 * Binary Tree Lib: helper routines
 *
 * author: Forrest Y. Yu <forrest.yu@gmail.com>, http://forrestyu.net/
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>
#include <err.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "colorful.h"
#include "bintreelib.h"

/******************************************************************************
 * routines
 ******************************************************************************/
/* a simple stack */
enum stack_op {PUSH, POP};
static void * simple_stack(enum stack_op op, void ** pp)
{
	static void * stack_pool[STACK_SIZE];
	static void ** top = stack_pool - 1;
	if (op == PUSH) {
		assert(top < stack_pool + STACK_SIZE);
		*(++top) = *pp;
		return *pp;
	} else {
		if (top >= stack_pool)
			return *top--;
		else
			return 0;
	}
}
void push(void * p)
{
	simple_stack(PUSH, &p);
}
void * pop(void)
{
	void ** pp = 0;
	return simple_stack(POP, pp);
}

void mkdir_if_needed(char * path, int verbose)
{
        char * p;
	if (verbose)
		fprintf(stderr, "%s", INFO_COLOR);
        for (p = path; *p; p++) {
                if (*p != '/')
                        continue;

                *p = 0;         /* change ``path'' temporarily */
		if (verbose)
			fprintf(stderr, "mkdir %s:\n    ", path);
                int ret = mkdir(path, 0755);
                if (ret) {
                        assert(ret == -1);
                        if (errno == EEXIST) {
				if (verbose)
					fprintf(stderr, "folder already exists\n");
                        } else {
				if (verbose)
					fprintf(stderr, "%s", NOCOLOR);
                                err(EXIT_FAILURE, "failed to mkdir ``%s''", path);
                        }
                } else {
			if (verbose)
				fprintf(stderr, "done\n");
                }
                *p = '/';       /* restore ``path'' */
        }
	if (verbose)
		fprintf(stderr, "%s\n", NOCOLOR);
}

/* routine via which the dot file is written */
void write_dot(const char * prefix, const char * figpath,
	       int id, const char *fmt, ...)
{
	static int flag = 0;
	static FILE * dotf[MAX_DOTF_NR];
	static char dotfilename[MAX_DOTF_NR][FILENAME_MAX];
	char cmd[CMD_MAX];

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
		sprintf(dotfilename[id], "%s%s%s (%03d).%s",
			figpath,
			figpath[strlen(figpath) - 1] == '/' ? "" : "/",
			prefix, id, GRAPHVIZ_FILE_EXT);

		mkdir_if_needed(dotfilename[id], 0);

		fprintf(stderr, "%sdot file: %s%s\n", INFO_COLOR,
			dotfilename[id], NOCOLOR);

		dotf[id] = fopen(dotfilename[id], "w");
		assert(dotf[id] != 0);

		fprintf(dotf[id], "digraph G {\n");

		fprintf(dotf[id],
			"\tsize=\"9,9\";"
			"\n");

		fprintf(dotf[id],
			"\tnode [shape=record"
			", fontname=Courier New"
			", penwidth=0.5"
			/* ", fixedsize=true" */
			/* ", width=0.6" */
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
			 "\"%s%s%s (%03d).%s\"",
			 GRAPHVIZ_CMD, GRAPHVIZ_FIG_TYPE, dotfilename[id],
			 figpath,
			 figpath[strlen(figpath) - 1] == '/' ? "" : "/",
			 prefix, id, GRAPHVIZ_FIG_TYPE);
		printf("%s$ %s%s%s\n", WHITE, WHITE, cmd, NOCOLOR);
		/* printf("%s$ %s%s%s\n", WHITE, GREEN, cmd, NOCOLOR); */
		assert(system(cmd) == 0);
	} else {
		fprintf(dotf[id], "\t");
		vfprintf(dotf[id], fmt, args);
	}

	va_end(args);
}

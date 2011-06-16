/*
 * RB Lib: helper routines for Red-Black Tree
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
#include "rbtlib.h"

const char RED_ATTR[] = ", style=\"filled\", fillcolor=\"red\"";
const char BLACK_ATTR[] = ", style=\"filled\", fillcolor=\"black\", fontcolor=\"white\"";
const char L_ATTR[] = "arrowhead=\"rnormal\", arrowsize=\"0.5\", color=\"cyan\"";
const char R_ATTR[] = "arrowhead=\"lnormal\", arrowsize=\"0.5\", color=\"magenta\"";
const char HL_ATTR[] = ", color=\"limegreen\", penwidth=\"3\""; /* highlight */
/* const char HL_ATTR[] = ""; */
const char NIL_ATTR[] = "label=\"\", width=\"0.15\", height=\"0.07\", shape=\"rect\", style=\"filled\", fillcolor=\"black\""; /* nil */

NODE * nil = 0;
int node_cnt = 0;

/******************************************************************************
 * routines
 ******************************************************************************/
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

	if (flag) {
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

		/* fprintf(dotf[id], */
			/* "\tsize=\"9,9\";" */
			/* "\n"); */

		fprintf(dotf[id],
			"\tnode [shape=circle"
			", fontname=\"DejaVu Sans Mono:style=Bold\""
			", fontsize=\"9\""
			", penwidth=0.5"
			", fixedsize=true"
			", width=0.25"
			", margin=\"0,0\""
			/* ", height=0.7" */
			", splines=\"compound\""
			"];\n");
	}

	assert(dotf[id] != 0);

	/* if the dot file becomes big, something must be wrong */
	int dot_f_max_size = 128*1024;
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
		printf("%s$ %s%s%s\n\n", WHITE, WHITE, cmd, NOCOLOR);
		/* printf("%s$ %s%s%s\n", WHITE, GREEN, cmd, NOCOLOR); */
		assert(system(cmd) == 0);
	} else {
		fprintf(dotf[id], "\t");
		vfprintf(dotf[id], fmt, args);
	}

	va_end(args);
}

NODE * alloc_node(int v, enum RB rb)
{
	static NODE node_pool[POOL_SIZE];
	static NODE * p = node_pool;
	assert(p < node_pool + POOL_SIZE);
	p->val = v;
	p->rb  = rb;
	p->L = nil;
	p->R = nil;
	p->P = nil;
	node_cnt++;
	return (p++);
}

void free_node(NODE * p)
{
	node_cnt--;
}

#define LEFT_X	(x - (hdelta) / 2.0)
#define LEFT_Y	(y - vdelta)
#define RIGHT_X	(x + (hdelta) / 2.0)
#define RIGHT_Y	(y - vdelta)

/* preorder traverse */
void preorder_traverse_recursive(const char * prefix, const char * path, int id,
				NODE * T, NODE * hl_node,
				double hdelta, double vdelta, double x, double y)
{
        if (T == nil)
		return;

	/* draw the node */
	write_dot(prefix, path, id,
		  "\"%p\" [label=\"%d\"%s%s, pos=\"%.5f,%.5f!\"];\n",
		  T, T->val,
		  T->rb == R ? RED_ATTR : BLACK_ATTR,
		  T == hl_node ? HL_ATTR : "",
		  x, y);

	/* printf("(%f,%f), %f,%f, (%f,%f), (%f,%f)\n", */
	/*        x, y, hdelta, vdelta, LEFT_X, LEFT_Y, RIGHT_X, RIGHT_Y); */

	preorder_traverse_recursive(prefix, path, id, T->L, hl_node,
				    hdelta / 2, vdelta, LEFT_X, LEFT_Y);
	preorder_traverse_recursive(prefix, path, id, T->R, hl_node,
				    hdelta / 2, vdelta, RIGHT_X, RIGHT_Y);

	/* draw the lines */
	if (T->L == nil) {
		write_dot(prefix, path, id,
			  "\"%p_L\" [%s, pos=\"%.5f,%.5f!\"];\n",
			  T, NIL_ATTR,
			  LEFT_X, LEFT_Y);
		write_dot(prefix, path, id, "\"%p\" -> \"%p_L\" [%s];\n",
			  T, T, L_ATTR);
	} else {
		if (T->L->P != T) {
			printf("%sT:%d, T->L:%d%s\n", RED, T->val, T->L->val, NOCOLOR);
		}
#if 1
		assert(T->L->P == T);
#else
		if (T->L->P != T) {
			fprintf(stderr,
				"%s%d's LEFT(%d)'s PARENT(%d) should"
				" have been %d%s\n", YELLOW,
				T->val, T->L->val, T->L->P->val, T->val,
				NOCOLOR);
		}
#endif
		write_dot(prefix, path, id, "\"%p\" -> \"%p\" [%s];\n",
			  T, T->L, L_ATTR);
	}

	if (T->R == nil) {
		write_dot(prefix, path, id,
			  "\"%p_R\" [%s, pos=\"%.5f,%.5f!\"];\n",
			  T, NIL_ATTR,
			  RIGHT_X, RIGHT_Y);
		write_dot(prefix, path, id, "\"%p\" -> \"%p_R\" [%s];\n",
			  T, T, R_ATTR);
	} else {
		assert(T->R->P == T);
		write_dot(prefix, path, id, "\"%p\" -> \"%p\" [%s];\n",
			  T, T->R, R_ATTR);
	}
}

int bh(NODE * p)
{
	assert(p->L == nil || p->R == nil);

	NODE * q;
	int h = 1;		/* bh(leaf) = 1 */
	for (q = p; q->P != nil; q = q->P)
		if(q->rb == B)
			h++;
	if (!(q->P == nil && q->rb == B))
		fprintf(stderr, "%swarning: root incorrect%s\n", YELLOW, NOCOLOR);
	return h;
}

void verify_rbtree(NODE * T, int depth)
{
	static int prev = INT_MIN;
	static int black_height = 0;
	if (depth == 0) {
		prev = INT_MIN;
		assert(black_height == 0);
	}
	if (T == nil)
		return;
	if (T->L == nil || T->R == nil) {
		if (black_height == 0) {
			black_height = bh(T);
			/* printf("bh(%d): %d\n", T->val, black_height); */
		} else {
			if (black_height != bh(T))
				fprintf(stderr,
					"%swarning: bh in trouble: %d != %d%s\n",
					YELLOW, bh(T), black_height, NOCOLOR);
		}
	}
	verify_rbtree(T->L, depth+1);
	if (T != nil && T->val < prev) {
		fprintf(stderr, "%sbad: %d < %d (T->val >= prev violated)%s\n", YELLOW, T->val, prev, NOCOLOR);
		/* abort(); */
	}
	prev = T->val;
#if 1
	if (T->rb == R && T->P && T->P->rb == R) {
		fprintf(stderr,
			"%swarning: both %d and its PARENT(%d) are RED%s\n",
			YELLOW, T->val, T->P->val, NOCOLOR);
	}
		
#endif
	verify_rbtree(T->R, depth+1);
	if (depth == 0) {
		black_height = 0;
	}
}

void rbtree2str(NODE * t)
{
	printf("(");
	if (t != nil) {
		printf("%d%s", t->val, t->rb == R ? "+" : "");
		if (t->L != nil || t->R !=nil) {
			rbtree2str(t->L);
			rbtree2str(t->R);
		}
	}
	printf(")");
}

void print_rbtree(const char * path, NODE * head, NODE * hl_node,
		  char * label_fmt, ...)
{
	verify_rbtree(head->L, 0);

	printf("%s", MAGENTA); rbtree2str(head->L); printf("%s\n", NOCOLOR);

        if (!head)
		return;

	static int id = 0;

	const char * prefix = "red_black tree";

	char label[DOT_LABEL_STR_LEN];
	va_list args;
	va_start(args, label_fmt);
	vsnprintf(label, DOT_LABEL_STR_LEN, label_fmt, args);
	va_end(args);

	write_dot(prefix, path, id, "label = \"red-black tree\\n%s\";\n",
		  label);

	double head_x = 0.0;
	double head_y = 1.0;
	double hdelta = 5;
	double vdelta = 0.5;
	/* draw head */
	write_dot(prefix, path, id,
		  "\"%p\" [label=\"root\", shape=\"diamond\""
		  ", fixedsize=false, pos=\"%.5f,%.5f!\"];\n",
		  head, head_x, head_y);
	assert(head->L);

        /* generate the graph */
	/* printf("%s", GREEN); */
        preorder_traverse_recursive(prefix, path, id, head->L, hl_node,
				    hdelta, vdelta, head_x, head_y - 1);
	/* printf("%s\n", NOCOLOR); */

	if (head->L == nil) {
		write_dot(prefix, path, id,
			  "\"%p_L\" [%s, pos=\"%.5f,%.5f!\"];\n",
			  head, NIL_ATTR,
			  head_x, head_y - 1);
		write_dot(prefix, path, id,
			  "\"%p\" -> \"%p_L\" [arrowhead=\"ornormal\", color=\"cyan\"];\n",
			  head, head);
	} else {
		write_dot(prefix, path, id,
			  "\"%p\" -> \"%p\" [arrowhead=\"ornormal\", color=\"cyan\"];\n",
			  head, head->L);
	}

        /* finish the graph */
        write_dot(prefix, path, id, "");
	id++;
}

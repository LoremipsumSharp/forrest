/*
 * Red-Black Tree
 *
 * author: Forrest Y. Yu <forrest.yu@gmail.com>, http://forrestyu.net/
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>
#include <assert.h>

#include "colorful.h"
#include "rbtlib.h"
#include "bst.h"
#include "CLRS.h"
#include "Sedgewick.h"

int dbg_level = 0;
void (*insert_val)(NODE * t, int v) = 0;
void (*delete_val)(NODE * t, int v) = 0;
void (*______________________________)(const char * path, NODE * head, NODE * hl_node, char * label_fmt, ...) = 0;
void (*______________________________I)(const char * path, NODE * head, NODE * hl_node, char * label_fmt, ...) = 0;
void (*______________________________D)(const char * path, NODE * head, NODE * hl_node, char * label_fmt, ...) = 0;

NODE * str2tree(const char ** pstr)
{
	/* examples:
	 *   ()
	 *   (2), (2+)
	 *   (2(1)(3)), (2+(1)(3)), (2(1+)(3+))
	 *   (2(1+)()), (2()(3+))
	 */
	int k;
	const char * p = *pstr;
	NODE * x = 0;

	assert(*p == '(');
	p++;
	assert(isdigit(*p) || *p == ')');
	if (isdigit(*p)) {
		k = 0;
		while (isdigit(*p))
			k = k * 10 + (*p++ - '0');
		if (*p == '+') {
			x = alloc_node(k, R);
			p++;
		} else {
			assert(*p == '(' || *p == ')');
			x = alloc_node(k, B);
		}

		if (*p == '(') {
			x->L = str2tree(&p);
			if (x->L != nil)
				x->L->P = x;

			x->R = str2tree(&p);
			if (x->R !=nil)
				x->R->P = x;
		}
	} else {
		x = nil;
	}
	assert(*p == ')');
	p++;

	printf("processed ");
	for (const char * q = *pstr; q < p; q++)
		printf("%s%c%s",  GREEN, *q, NOCOLOR);
	printf("\n");
	*pstr = p;

	return x;
}

NODE * init_rbtree(const char * tstr)
{
	nil = alloc_node(-1, B); /* the sentinel */
	nil->P = nil;
	nil->L = nil;
	nil->R = nil;

	NODE * head= alloc_node(INT_MAX, B);
	head->P = nil;
	head->L = str2tree(&tstr);
	head->R = nil;

	assert(*tstr == '\0');

	return head;
}

void insert_CLRS(NODE * t, int v)
{
	if (dbg_level == 0) {
		fprintf(stderr, "%s%d%s, ", GREEN, v, NOCOLOR);
		rb_insert(t, alloc_node(v, R));
		verify_rbtree(t->L, 0);
	} else {
		printf("%sinsert %d%s\n", GREEN, v, NOCOLOR);
		rb_insert(t, alloc_node(v, R));
		______________________________("./fig/", t, t, "inserted %d", v);
	}
}

void delete_CLRS(NODE * t, int v)
{
	NODE * to_del = rb_search(t, v);
	assert(to_del);

	if (dbg_level == 0) {
		fprintf(stderr, "%s~%d%s, ", RED, v, NOCOLOR);
		rb_delete(t, to_del);
		verify_rbtree(t->L, 0);
	} else {
		printf("%sdelete %d%s\n", RED, v, NOCOLOR);
		rb_delete(t, to_del);
		______________________________("./fig/", t, t, "deleted %d", v);
	}
}

void insert_Sedgewick(NODE * t, int v)
{
	if (dbg_level == 0) {
		fprintf(stderr, "%s%d%s, ", GREEN, v, NOCOLOR);
		Sedgewick_rb_insert(t, v);
		verify_rbtree(t->L, 0);
	} else {
		printf("%sinsert %d%s\n", GREEN, v, NOCOLOR);
		Sedgewick_rb_insert(t, v);
		______________________________("./fig/", t, t, "inserted %d", v);
	}
}

void delete_Sedgewick(NODE * t, int v)
{
	assert(rb_search(t, v));
	if (dbg_level == 0) {
		fprintf(stderr, "%s~%d%s, ", RED, v, NOCOLOR);
		Sedgewick_rb_delete(t, v);
		verify_rbtree(t->L, 0);
	} else {
		printf("%sdelete %d%s\n", RED, v, NOCOLOR);
		Sedgewick_rb_delete(t, v);
		______________________________("./fig/", t, t, "deleted %d", v);
	}
}
void print_nothing(const char * path, NODE * head, NODE * hl_node,
		   char * label_fmt, ...)
{
	;	/* this is a dummy function, so let's do nothing */
}

void print_usage(char * bin)
{
	char * indent0 = "  ";
	char * indent1 = "        ";
	printf("Usage: %s [OPTION]\n", bin);
	printf("%s%s, %s\n%s%s\n",
	       indent0, "-h", "--help",
	       indent1, "print this help and exit");
	printf("%s%s, %s\n%s%s\n",
	       indent0, "-a [CLRS|Sedgewick]", "--algorithm [CLRS|Sedgewick]",
	       indent1, "specify the desired algorithm");
	printf("%s%s, %s\n%s%s\n",
	       indent0, "-d N", "--debug-level N",
	       indent1, "specify the debug level (default is 0)");
	printf("%s%s, %s\n%s%s\n",
	       indent0, "-t \"tree-string\"", "--tree \"tree-string\"",
	       indent1, "specify the input tree");
	printf("%s%s, %s\n%s%s\n",
	       indent0, "-l N", "--limit N",
	       indent1, "specify how many tests will be performed (default is 100)");
	printf("\n");
	printf("%s\n", "* If no -a is given, use Sedgewick as the default algorithm.");
	printf("%s\n", "* tree-string sample: (11(2+(1)(7(5+)(8+)))(14()(15+)))");
	printf("%s\n", "* debug level:");
	printf("%s%s\n", indent1, "0: output the final tree only");
	printf("%s%s\n", indent1, "1: output a tree whenever a node is inserted/deleted");
	printf("%s%s\n", indent1, "2: demostrate the inserting process");
	printf("%s%s\n", indent1, "3: demostrate the deleting process");
	printf("%s%s\n", indent1, "4: demostrate inserting and deleteing processes");
	exit(0);
}

int main(int argc, char * argv[])
{
	const char * tree_str = 0;
	NODE * t = 0;
	int limit = -1;
	opterr = 0;
	______________________________  = print_nothing;
	______________________________I = print_nothing;
	______________________________D = print_nothing;

	while (1) {
		int c;
		static struct option long_options[] = {
			{"help",           no_argument,       0, 'h'},
			{"algorithm",      required_argument, 0, 'a'},
			{"debug-level",    required_argument, 0, 'd'},
			{"tree",           required_argument, 0, 't'},
			{"limit",          required_argument, 0, 'l'},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;
     
		c = getopt_long(argc, argv, "ha:t:d:l:",
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

		case 'h':
			print_usage(argv[0]);
			break;
     
		case 'a':
			if (strcmp(optarg, "CLRS") == 0) {
				insert_val = insert_CLRS;
				delete_val = delete_CLRS;
			} else if (strcmp(optarg, "Sedgewick") == 0) {
				insert_val = insert_Sedgewick;
				delete_val = delete_Sedgewick;
			} else {
				abort();
			}
			break;
     
		case 'd':
			dbg_level = atoi_or_abort(optarg);
			if (dbg_level == 1) {
				______________________________  = print_rbtree;
			} else if (dbg_level == 2) {
				______________________________  = print_rbtree;
				______________________________I = print_rbtree;
			} else if (dbg_level == 3) {
				______________________________  = print_rbtree;
				______________________________D = print_rbtree;
			} else if (dbg_level == 4) {
				______________________________  = print_rbtree;
				______________________________I = print_rbtree;
				______________________________D = print_rbtree;
			} else {
				; /* use default */
			}
			break;
     
		case 't':
			tree_str = optarg;
			break;
          
		case 'l':
			limit = atoi_or_abort(optarg);
			break;
          
		case '?':
			/* getopt_long already printed an error message. */
			printf("%s%s%s\n", RED_B, argv[optind-1], NOCOLOR);
			break;
     
		default:
			abort();
		}
	}

	printf("%sDebug Level: %s%d%s\n", YELLOW, CYAN, dbg_level, NOCOLOR);

	static const char * default_tree[] = {/* p.275 */ "(26(17+(14(10+(7(3+)())(12))(16(15+)()))(21(19()(20+))(23)))(41(30+(28)(38(35+)(39+)))(47)))",
					      /* p.282 */ "(11(2+(1)(7(5+)(8+)))(14()(15+)))",
					      ""};
	if (!tree_str) {
		tree_str = default_tree[0];
		printf("%sUsing default tree: %s%s%s\n", YELLOW, CYAN, tree_str, NOCOLOR);
	}
	t = init_rbtree(tree_str);
	assert(t->L->P == nil);

        ______________________________("./fig/", t, t, "init");
	printf("Node count: %d\n", node_cnt);

	/* default algorithms */
	if (insert_val == 0)
		insert_val = insert_Sedgewick;
	if (delete_val == 0)
		delete_val = delete_Sedgewick;

	/* printf("optind: %d, argc: %d\n", optind, argc); */
	/* for (int j = 0; j < argc; j++) */
	/* 	printf("        %s\n", argv[j]); */
	if (optind < argc)
	{
		while (optind < argc) {
			char * a = argv[optind++];
			if (*a == '~')
				delete_val(t, atoi_or_abort(a+1));
			else
				insert_val(t, atoi_or_abort(a));
			verify_rbtree(t->L, 0);
		}
	} else {
		srand(time(NULL));
		if (limit == -1)
			limit = 100;
		printf("%sWill run %s%d%s tests.%s\n", YELLOW, CYAN, limit, YELLOW, NOCOLOR);
		while (t->L != nil && limit--) {
			int k = rand() % 50;
			int r = rand();
			if (dbg_level != 0)
				printf("random: %d\n", r);
			if (r % 2 != 0)
				insert_val(t, k);
			else
				if (rb_search(t, k) != 0)
					delete_val(t, k);
		}
		verify_rbtree(t->L, 0);
	}

	fprintf(stderr, "\n");
	print_rbtree("./fig/", t, t, "END");
	printf("Node count: %d\n", node_cnt);
	return 0;
}

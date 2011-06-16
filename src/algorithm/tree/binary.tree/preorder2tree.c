/*
 * http://stackoverflow.com/questions/5320541/binary-tree-construction-from-preorder/5327796
 * 
 * It is said that this is an Amazon interview question.
 *
 * There is a binary tree with the following properties:
 *  - All of its inner node have the value 'N', and all the leaves have the value 'L'.
 *  - Every node either has two children or has no child.
 *  
 * Given its preorder, construct the tree and return the root node.
 *
 * author: Forrest Y. Yu <forrest.yu@gmail.com>, http://forrestyu.net/
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#define MAX_DOTF_NR		128

/* colors */
const char BLACK[]  = {033, '[', '3', '0', 'm', 0};
const char RED[]    = {033, '[', '3', '1', 'm', 0};
const char GREEN[]  = {033, '[', '3', '2', 'm', 0};
const char YELLOW[] = {033, '[', '3', '3', 'm', 0};
const char BLUE[]   = {033, '[', '3', '4', 'm', 0};
const char MAGENTA[]= {033, '[', '3', '5', 'm', 0};
const char CYAN[]   = {033, '[', '3', '6', 'm', 0};
const char WHITE[]  = {033, '[', '3', '7', 'm', 0};
const char NOCOLOR[]= {033, '[',      '0', 'm', 0};

struct tnode {
	int info;
	int val;
	struct tnode * L;
	struct tnode * R;
};

typedef struct tnode NODE;

/* routine via which the dot file is written */
void write_dot(int id, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	assert(id >= 0 && id < MAX_DOTF_NR);

	static int flag = 0;
	static FILE * dotf[MAX_DOTF_NR];
	static char dotfilename[MAX_DOTF_NR][FILENAME_MAX];
	if (flag) {
		int i;
		for (i = 0; i < MAX_DOTF_NR; i++)
			dotf[i] = 0;
		flag = 1;
	}
	if (!dotf[id]) {
		char * p;
		strcpy(dotfilename[id], __FILE__);
		for (p = dotfilename[id] + strlen(dotfilename[id]); *p != '.'; p--);
		sprintf(p, "%02d.dot", id);
		fprintf(stderr, "\ndot file: %s\n", dotfilename[id]);

		dotf[id] = fopen(dotfilename[id], "w");
		assert(dotf[id]);

		fprintf(dotf[id], "digraph example {\n");

		fprintf(dotf[id],
			"\tnode [shape=record"
			", fontname=Courier New"
			", penwidth=0.5"
			", height=0.3"
			"];\n");
	}

	assert(dotf[id]);
	if (!fmt[0]) {
		fprintf(dotf[id], "}\n");
		fclose(dotf[id]);
		dotf[id] = 0;
		char cmd[FILENAME_MAX + 128];
		sprintf(cmd, "dot -Tpng %s -o \"preorder2tree (%02d).png\"", dotfilename[id], id);
		printf("\n%s$ %s%s%s\n", WHITE, GREEN, cmd, NOCOLOR);
		assert(system(cmd) == 0);
	} else {
		fprintf(dotf[id], "\t");
		vfprintf(dotf[id], fmt, args);
	}

	va_end(args);
}

/* what to do when visiting a node */
void visit(int id, NODE * p)
{
	printf("%s%c(%c)%s", RED, p->info, p->val, NOCOLOR);
	write_dot(id, "\"%X\" [label=\"<l>|<m> %c|<r>\"];\n", (int)p, p->info);
}

/* inorder traverse */
void inorder_traverse_recursive(int id, NODE * T)
{
	const char L_ATTR[] = "arrowhead=\"rnormal\", color=\"blue\"";
	const char R_ATTR[] = "arrowhead=\"lnormal\", color=\"red\"";

	if (!T)
		return;

	inorder_traverse_recursive(id, T->L);
	visit(id, T);
	if (T->L)
		write_dot(id, "\"%X\":l -> \"%X\":m [%s];\n", (int)T, (int)T->L, L_ATTR);
	if (T->R)
		write_dot(id, "\"%X\":r -> \"%X\":m [%s];\n", (int)T, (int)T->R, R_ATTR);
	inorder_traverse_recursive(id, T->R);
}

void print_tree(int id, NODE * head)
{
	assert(head);

	/* generate the graph */
	inorder_traverse_recursive(id, head);

	/* finish the graph */
	write_dot(id, "");
	printf("tree %d outputted.\n", id);
}
/******************************************************************************/

/* test case 1 */
/* #define NODES_NR 7 */
/* int info[] = {'A', 'B', 'D', 'E', 'F', 'G', 'C'}; */
/* int val[]  = {'N', 'N', 'L', 'N', 'L', 'L', 'L'}; */

/* test case 2 */
#define NODES_NR 11
int info[] = {'A', 'B', 'D', 'M', 'C', 'E', 'K', 'G', 'F', 'H', 'J'};
int val[]  = {'N', 'N', 'L', 'L', 'N', 'N', 'L', 'L', 'N', 'L', 'L'};


NODE nodes[NODES_NR];
NODE * STACK[NODES_NR];
int top = 0;

void init_nodes(void)
{
	assert(sizeof(info) / sizeof(info[0]) == NODES_NR);
	assert(sizeof(val) / sizeof(val[0]) == NODES_NR);
	int i;
	for (i = 0; i < NODES_NR; i++) {
		nodes[i].info = info[i];
		nodes[i].val = val[i];
		nodes[i].L = 0;
		nodes[i].R = 0;
	}
}

NODE * next_node(void)
{
	static int i = 0;
	if (i < NODES_NR)
		return (&nodes[i++]);
	else
		return 0;
}

void push(NODE * p)
{
	STACK[top++] = p;
}

NODE * pop(void)
{
	return STACK[--top];
}

NODE * preorder2tree(void)
{
        NODE * head = next_node();
        NODE * p = head;
        NODE * q;

        while (1) {
                q = next_node();
                if (!q)
                        break;
                /* printf("p: %c(%c), q: %c(%c)\n", p->info, p->val, q->info, q->val); */
                /* possibilities of adjacent nodes:
                 * NN, NL, LN, LL
                 */
                if (p->val == 'N') {
                        p->L = q;
                        if (q->val == 'N') { /* NN */
                                push(p);
                                p = q;
                        } else {             /* NL */
                                q = next_node();
                                p->R = q;
                                p = q;
                        }
                } else {                     /* LN or LL */
                        p = pop();
                        p->R = q;
                        p = q;
                }
        }
        return head;
}

int main(void)
{
	init_nodes();
	NODE * p = preorder2tree();
	print_tree(0, p);
	return 0;
}

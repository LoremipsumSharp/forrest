/*
 * given a binary tree in which every node has three links: P, L and R,
 * do inorder traversal in three ways:
 * - recursive
 * - non-recursive
 * - non-recursive without stack
 *
 * author: Forrest Y. Yu <forrest.yu@gmail.com>, http://forrestyu.net/
 */

#include <stdio.h>
#include <assert.h>

#include "colorful.h"
#include "bintreelib.h"

typedef struct _node {
	int            val;
	struct _node * L;
	struct _node * R;
	struct _node * P;
} NODE;

NODE * alloc_node(int v)
{
	static NODE node_pool[POOL_SIZE];
	static NODE * p = node_pool;
	assert(p < node_pool + POOL_SIZE);
	p->val = v;
	p->L = 0;
	p->R = 0;
	p->P = 0;
	return (p++);
}

/* inorder traverse */
void inorder_traverse_recursive(const char * prefix, const char * path, int id, NODE * T)
{
        if (T == 0)
		return;

	inorder_traverse_recursive(prefix, path, id, T->L);

	write_dot(prefix, path, id, "\"%X\" [label=\"<l>|<m> %c|<r>\"];\n", (int)T, T->val);
	putchar(T->val);

        const char L_ATTR[] = "arrowhead=\"rnormal\", color=\"blue\"";
        const char R_ATTR[] = "arrowhead=\"lnormal\", color=\"red\"";

	if (T->L)
		write_dot(prefix, path, id, "\"%X\":l -> \"%X\":m [%s];\n", (int)T, (int)T->L, L_ATTR);

	if (T->R)
		write_dot(prefix, path, id, "\"%X\":r -> \"%X\":m [%s];\n", (int)T, (int)T->R, R_ATTR);

	inorder_traverse_recursive(prefix, path, id, T->R);
}

void print_binary_tree(const char * prefix, const char * path, int id, NODE * head)
{
        if(!head)
		return;

        /* generate the graph */
	printf("%s", GREEN);
        inorder_traverse_recursive(prefix, path, id, head);
	printf("%s\n", NOCOLOR);

        /* finish the graph */
        write_dot(prefix, path, id, "");
}

NODE * init_binary_tree(void)
{
	/*            A
         *          /   \
         *        B       C
         *      /       /   \
         *    D       E       F
         *             \     / \
         *              G   H   J
         */
        NODE * A = alloc_node('A');
        NODE * B = alloc_node('B');
        NODE * C = alloc_node('C');
        NODE * D = alloc_node('D');
        NODE * E = alloc_node('E');
        NODE * F = alloc_node('F');
        NODE * G = alloc_node('G');
        NODE * H = alloc_node('H');
        NODE * J = alloc_node('J');
        A->L = B; A->R = C;
        B->L = D;
        C->L = E; C->R = F;
        E->R = G;
        F->L = H; F->R = J;

	A->P = 0;
	B->P = A;
	C->P = A;
	D->P = B;
	E->P = C;
	F->P = C;
	G->P = E;
	H->P = F;
	J->P = F;

	return A;
}

void visit(NODE * p)
{
	if (!p)
		return;
	printf("%c", p->val);
	fflush(stdout);
}

void inorder_nonrecursive(NODE * T)
{
	NODE * p = T;

	printf("%s", CYAN);
	while (1) {
		while (p) {
			push(p);
			p = p->L;
		}

		p = pop();
		if (!p)
			break;
		visit(p);

		p = p->R;
	}
	printf("%s\n", NOCOLOR);
}

void inorder_nonrecursive_without_stack(NODE * T)
{
	NODE * p = T;

	NODE * head = alloc_node('^');
	head->L = T;
	head->R = head;
	head->P = 0;

	assert(T->P == 0);
	T->P = head;		/* change the Parent Link of T temporarily */

	if (p == 0)
		return;

	while (p->L)
		p = p->L;

	printf("%s", RED);
	while (p != head) {
		visit(p);

		if (p->R) {	/* p has right subtree */
			p = p->R;
			while (p->L)
				p = p->L;
		} else {	/* p has no right subtree */
			while (p->P->L != p)
				p = p->P;
			p = p->P;
		}
	}
	T->P = 0;		/* restore the Parent Link of T */

	printf("%s\n", NOCOLOR);
}

int main(int argc, char * argv[])
{
	NODE * t = init_binary_tree();
        print_binary_tree("binary tree", "./fig/inorder/", 0, t);
	inorder_nonrecursive(t);
	inorder_nonrecursive_without_stack(t);
	return 0;
}

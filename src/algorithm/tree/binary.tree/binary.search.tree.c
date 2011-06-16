/*
 * Binary Search Tree (BST)
 *
 * author: Forrest Y. Yu <forrest.yu@gmail.com>, http://forrestyu.net/
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
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

void free_node(NODE * p)
{
	return;			/* do nothing */
}

/* inorder traverse */
void inorder_traverse_recursive(const char * prefix, const char * path, int id, NODE * T, NODE * hl_node)
{
        const char L_ATTR[] = "arrowhead=\"rnormal\", color=\"blue\"";
        const char R_ATTR[] = "arrowhead=\"lnormal\", color=\"red\"";
	const char HL_ATTR[] = ", style=\"filled\", fillcolor=\"gray90\""; /* highlight */
        const char INVIS_ATTR[] = "style=\"invis\"";
        /* const char INVIS_ATTR[] = ""; */

        if (T == 0)
		return;

	inorder_traverse_recursive(prefix, path, id, T->L, hl_node);

	write_dot(prefix, path, id, "\"%X\" [label=\"<l>|<m> %d|<r>\"%s];\n",
		  (int)T, T->val,
		  T == hl_node ? HL_ATTR : "");
	printf("%s%d%s ", T == hl_node ? "<" : "", T->val, T == hl_node ? ">" : "");

	if (T->L) {
		if (T->L->P != T) {
			printf("%sT:%d, T->L:%d%s\n", RED, T->val, T->L->val, NOCOLOR);
		}
		assert(T->L->P == T);
		write_dot(prefix, path, id, "\"%X\":l -> \"%X\":m [%s];\n", (int)T, (int)T->L, L_ATTR);
	} else {
		write_dot(prefix, path, id, "\"%X_L\" [label=\"<l>|<m> %d|<r>\", %s];\n", (int)T, T->val, INVIS_ATTR);
		write_dot(prefix, path, id, "\"%X\":l -> \"%X_L\":m [%s];\n", (int)T, (int)T, INVIS_ATTR);

		/* write_dot(prefix, path, id, "\"%X_L_L\" [label=\"<l>|<m> %d|<r>\", %s];\n", (int)T, T->val, INVIS_ATTR); */
		/* write_dot(prefix, path, id, "\"%X_L_R\" [label=\"<l>|<m> %d|<r>\", %s];\n", (int)T, T->val, INVIS_ATTR); */
		/* write_dot(prefix, path, id, "\"%X_L\":l -> \"%X_L_L\":m [%s];\n", (int)T, (int)T, INVIS_ATTR); */
		/* write_dot(prefix, path, id, "\"%X_L\":r -> \"%X_L_R\":m [%s];\n", (int)T, (int)T, INVIS_ATTR); */
	}

	if (T->R) {
		assert(T->R->P == T);
		write_dot(prefix, path, id, "\"%X\":r -> \"%X\":m [%s];\n", (int)T, (int)T->R, R_ATTR);
	} else {
		write_dot(prefix, path, id, "\"%X_R\" [label=\"<l>|<m> %d|<r>\", %s];\n", (int)T, T->val, INVIS_ATTR);
		write_dot(prefix, path, id, "\"%X\":r -> \"%X_R\":m [%s];\n", (int)T, (int)T, INVIS_ATTR);

		/* write_dot(prefix, path, id, "\"%X_R_L\" [label=\"<l>|<m> %d|<r>\", %s];\n", (int)T, T->val, INVIS_ATTR); */
		/* write_dot(prefix, path, id, "\"%X_R_R\" [label=\"<l>|<m> %d|<r>\", %s];\n", (int)T, T->val, INVIS_ATTR); */
		/* write_dot(prefix, path, id, "\"%X_R\":l -> \"%X_R_L\":m [%s];\n", (int)T, (int)T, INVIS_ATTR); */
		/* write_dot(prefix, path, id, "\"%X_R\":r -> \"%X_R_R\":m [%s];\n", (int)T, (int)T, INVIS_ATTR); */
	}

	inorder_traverse_recursive(prefix, path, id, T->R, hl_node);
}

void verify_binary_search_tree(NODE * T, int depth)
{
	static int prev = INT_MIN;
	if (depth == 0)
		prev = INT_MIN;
	if (!T)
		return;
	verify_binary_search_tree(T->L, depth+1);
	assert(T->val >= prev);
	prev = T->val;
	verify_binary_search_tree(T->R, depth+1);
}

void print_binary_tree(const char * path, NODE * head, NODE * hl_node,
		       char * label_fmt, ...)
{
	verify_binary_search_tree(head, 0);

        if (!head)
		return;

	static int id = 0;

	const char * prefix = "binary search tree";

	char label[DOT_LABEL_STR_LEN];
	va_list args;
	va_start(args, label_fmt);
	vsnprintf(label, DOT_LABEL_STR_LEN, label_fmt, args);
	va_end(args);

	write_dot(prefix, path, id, "label = \"binary search tree\\n%s\";\n",
		  label);

        /* generate the graph */
	printf("%s", GREEN);
        inorder_traverse_recursive(prefix, path, id, head, hl_node);
	printf("%s\n", NOCOLOR);

        /* finish the graph */
        write_dot(prefix, path, id, "");
	id++;
}

NODE * init_binary_tree(void)
{
	/*                 15
         *              /      \
         *           /            \
         *         6               18
         *       /   \           /    \
         *      /     \         /      \
         *     3       7       17      20
         *    / \       \
         *   /   \       \
         *  2     4      13
	 *               /
	 *              9
         */
        NODE * n02 = alloc_node(2);
        NODE * n03 = alloc_node(3);
        NODE * n04 = alloc_node(4);
        NODE * n06 = alloc_node(6);
        NODE * n07 = alloc_node(7);
        NODE * n09 = alloc_node(9);
        NODE * n13 = alloc_node(13);
        NODE * n15 = alloc_node(15);
        NODE * n17 = alloc_node(17);
        NODE * n18 = alloc_node(18);
        NODE * n20 = alloc_node(20);
        n15->L = n06; n15->R = n18;
        n06->L = n03; n06->R = n07;
        n03->L = n02; n03->R = n04;
        n07->R = n13;
        n13->L = n09;
	n18->L = n17; n18->R = n20;

	n15->P = 0;
	n06->P = n15; n18->P = n15;
	n03->P = n06; n07->P = n06;
	n02->P = n03; n04->P = n03;
	n13->P = n07;
	n09->P = n13;
	n17->P = n18; n20->P = n18;

	return n15;
}

NODE * tree_search(NODE * x, int k)
{
	if (!x)
		return NULL;	/* not found */

	if (k == x->val)
		return x;

	if (k < x->val)
		return tree_search(x->L, k);
	else
		return tree_search(x->R, k);
}

NODE * iterative_tree_search(NODE * x, int k)
{
	while (x) {
		if (k == x->val)
			return x;
		if (k < x->val)
			x = x->L;
		else
			x = x->R;
	}
	return NULL;		/* not found */
}

NODE * tree_minimum(NODE * x)
{
	if (!x)
		return NULL;

	while (x->L)
		x = x->L;

	return x;
}

NODE * tree_maximum(NODE * x)
{
	if (!x)
		return NULL;

	while (x->R)
		x = x->R;

	return x;
}

NODE * recursive_tree_minimum(NODE * x)
{
	if (!x)
		return NULL;
	if (x->L)
		return recursive_tree_minimum(x->L);
	else
		return x;
}

NODE * recursive_tree_maximum(NODE * x)
{
	if (!x)
		return NULL;
	if (x->R)
		return recursive_tree_maximum(x->R);
	else
		return x;
}

NODE * tree_successor(NODE * x)
{
	if (!x)
		return NULL;

	if (x->R) {
		return tree_minimum(x->R);
	} else {
		NODE * p = x->P;
		while (p) {
			if (p->L == x)
				break;
			x = p;
			p = x->P;
		}
		return p;
	}
}

NODE * tree_predecessor(NODE * x)
{
	if (!x)
		return NULL;

	if (x->L) {
		return tree_maximum(x->L);
	} else {
		NODE * p = x->P;
		while (p) {
			if (p->R == x)
				break;
			x = p;
			p = x->P;
		}
		return p;
	}
}

void tree_insert(NODE * x, NODE * z)
{
	NODE * p;
	while (x) {
		p = x;
		if (z->val < x->val)
			x = x->L;
		else
			x = x->R;
	}
	if (z->val < p->val) {
		assert(p->L == 0);
		p->L = z;
	} else {
		assert(p->R == 0);
		p->R = z;
	}
	z->P = p;
	assert(z->L == 0 && z->R == 0);
}

void recursive_tree_insert(NODE * x, NODE * z)
{
	if (z->val < x->val)
		if (x->L)
			recursive_tree_insert(x->L, z);
		else {
			x->L = z;
			z->P = x;
			assert(z->L == 0 && z->R == 0);
		}
	else
		if (x->R)
			recursive_tree_insert(x->R, z);
		else {
			x->R = z;
			z->P = x;
			assert(z->L == 0 && z->R == 0);
		}
}

NODE * tree_delete(NODE * x, NODE * z)
{
	NODE * head = x;
	if (z->L == 0 || z->R == 0) {
		NODE * q = z->L ? z->L : z->R;
		if (q)
			q->P = z->P;
		if (z->P) {
			if (z->P->L == z) {
				z->P->L = q;
			} else {
				assert(z->P->R == z);
				z->P->R = q;
			}
		} else {
			head = q;
		}
	} else {
		NODE * q = tree_successor(z);
		z->val = q->val;
		tree_delete(x, q);
		head = x;
	}
	free_node(z);
	return head;
}

/* from Introduction to Algorithms, p.262
 *
 * TREE-DELETE(T, z)
 *  1  if left[z] = NIL or right[z] = NIL
 *  2      then y ← z
 *  3      else y ← TREE-SUCCESSOR(z)
 *  4  if left[y] ≠ NIL
 *  5      then x ← left[y]
 *  6      else x ← right[y]
 *  7  if x ≠ NIL
 *  8      then p[x] ← p[y]
 *  9  if p[y] = NIL
 * 10      then root[T] ← x
 * 11      else if y = left[p[y]]
 * 12              then left[p[y]] ← x
 * 13              else right[p[y]] ← x
 * 14  if y ≠ z
 * 15      then key[z] ← key[y]
 * 16           copy y's satellite data into z
 * 17  return y
 */
NODE * book_tree_delete(NODE * T, NODE * z)
{
	NODE * root = T;
	NODE * y = (z->L == 0 || z->R == 0) ? z : tree_successor(z);
	NODE * x = y->L ? y->L : y->R;
	if (x)
		x->P = y->P;
	if (y->P == 0)
		root = x;
	else if (y == y->P->L)
		y->P->L = x;
	else
		y->P->R = x;
	if (y != z)
		z->val = y->val;
	return root;
}

/* @see Exercise 12.3-4 */
NODE * book_tree_delete_improved(NODE * T, NODE * z)
{
	NODE * root = T;
	NODE * y = (z->L == 0 || z->R == 0) ? z : tree_successor(z);
	NODE * x = y->L ? y->L : y->R;
	if (x)
		x->P = y->P;
	if (y->P == 0)
		root = x;
	else if (y == y->P->L)
		y->P->L = x;
	else
		y->P->R = x;
	if (y != z) {
		y->P = z->P;
		y->L = z->L;
		y->R = z->R;
		if (z->P == 0) {
			root = y;
		} else if (z->P->L == z) {
			z->P->L = y;
		} else {
			assert(z->P->R == z);
			z->P->R = y;
		}
		if (y->L) {
			assert(y->L->P == z);
			y->L->P = y;
		}
		if (y->R) {
			assert(y->R->P == z);
			y->R->P = y;
		}
	}
	return root;
}

int main(int argc, char * argv[])
{
	NODE * t = init_binary_tree();
        print_binary_tree("./fig/binary.search.tree/",
			  t, 0, "init");

	NODE * p;

	p = tree_minimum(t);
	print_binary_tree("./fig/binary.search.tree/min.max/",
			  t, p, "tree minimum");

	p = recursive_tree_minimum(t);
	print_binary_tree("./fig/binary.search.tree/min.max/",
			  t, p, "recursive tree minimum");

	p = tree_maximum(t);
	print_binary_tree("./fig/binary.search.tree/min.max/",
			  t, p, "tree maximum");

	p = recursive_tree_maximum(t);
	print_binary_tree("./fig/binary.search.tree/min.max/",
			  t, p, "recursive tree maximum");

	int to_search[] = {7, 13, 17, 55};

	for (int i = 0; i < sizeof(to_search) / sizeof(to_search[0]); i++) {
		p = tree_search(t, to_search[i]);
		print_binary_tree("./fig/binary.search.tree/search/",
				  t, p, "tree search %d", to_search[i]);

		p = iterative_tree_search(t, to_search[i]);
		print_binary_tree("./fig/binary.search.tree/search/",
				  t, p, "iterative tree search %d", to_search[i]);

		NODE * q;

		q = tree_minimum(p);
		print_binary_tree("./fig/binary.search.tree/min.max/",
				  p, q, "tree minimum");

		q = recursive_tree_minimum(p);
		print_binary_tree("./fig/binary.search.tree/min.max/",
				  p, q, "recursive tree minimum");

		q = tree_maximum(p);
		print_binary_tree("./fig/binary.search.tree/min.max/",
				  p, q, "tree maximum");

		q = recursive_tree_maximum(p);
		print_binary_tree("./fig/binary.search.tree/min.max/",
				  p, q, "recursive tree maximum");

		q = tree_successor(p);
		print_binary_tree("./fig/binary.search.tree/predecessor.successor/",
				  t, p, "tree successor %d$: %d",
				  to_search[i], q ? q->val : 0);

		q = tree_predecessor(p);
		print_binary_tree("./fig/binary.search.tree/predecessor.successor/",
				  t, p, "tree predecessor $%d: %d",
				  to_search[i], q ? q->val : 0);
	}

	NODE * z;

	int to_ins[] = {11, 16};

	for (int i = 0; i < sizeof(to_ins) / sizeof(to_ins[0]); i++) {
		z = alloc_node(to_ins[i]);
#if 1
		tree_insert(t, z);
		print_binary_tree("./fig/binary.search.tree/insert.delete/",
				  t, z, "tree insert: %d", z->val);
#else
		recursive_tree_insert(t, z);
		print_binary_tree("./fig/binary.search.tree/insert.delete/",
				  t, z, "recursive tree insert: %d", z->val);
#endif
	}

	int to_del[] = {6, 7, 13, 17, 11, 16, 15, 3, 20, 2, 4, 9, 18};

	for (int i = 0; i < sizeof(to_del) / sizeof(to_del[0]); i++) {
		p = tree_search(t, to_del[i]);
		assert(p);
#if 0
		t = tree_delete(t, p);
		if (t)
			print_binary_tree("./fig/binary.search.tree/insert.delete/",
					  t, 0, "tree delete: %d", to_del[i]);
#else
#if 0
		t = book_tree_delete(t, p);
		if (t)
			print_binary_tree("./fig/binary.search.tree/insert.delete/",
					  t, 0, "tree delete @ book: %d", to_del[i]);
#else
		t = book_tree_delete_improved(t, p);
		if (t)
			print_binary_tree("./fig/binary.search.tree/insert.delete/",
					  t, 0, "tree delete @ book (improved): %d", to_del[i]);
#endif
#endif
		else
			printf("the tree was just destroyed\n");
	}

	return 0;
}

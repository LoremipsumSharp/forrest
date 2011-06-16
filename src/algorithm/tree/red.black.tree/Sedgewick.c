/*
 * implementation of R. Sedgewick's Red-Black Tree Algorithm
 *
 * author: Forrest Y. Yu <forrest.yu@gmail.com>, http://forrestyu.net/
 *
 * @see Robert Sedgewick, Algorithms in C, Parts 1-4 (Fundamental Algorithms, Data Structures, Sorting, Searching)
 */

#include <stdio.h>
#include <assert.h>

#include "colorful.h"
#include "rbtlib.h"

/* 
 *          .--- RBhead
 *          |
 *          V
 *         root
 *         /  \
 *       ...  ...
 */
static NODE * RBhead = 0;
#define root (RBhead->L)

/* from Robert Sedgewick's Alorithm in C */
NODE * rotR(NODE * h)
{
	/* NODE * x = h->L; */
	/* h->L = x->R; */
	/* x->R = h; */
	/* return x; */
	return rotate_right(RBhead, h);
}
NODE * rotL(NODE * h)
{
	/* NODE * x = h->R; */
	/* h->R = x->L; */
	/* x->L = h; */
	/* return x; */
	return rotate_left(RBhead, h);
}
NODE * RBinsert(NODE * h, int item, int sw)
{
	int v = item;
	if (h == nil)
		return alloc_node(item, R);
	if (h->L->rb == R && h->R->rb == R) {
		h->rb = R;
		h->L->rb = B;
		h->R->rb = B;
	}
	if (v < h->val) {
		______________________________I("./fig/", RBhead, h, "inserting %d (ln%d)", v, __LINE__);
		h->L = RBinsert(h->L, item, 0);
		h->L->P = h;
		______________________________I("./fig/", RBhead, h, "inserting %d (ln%d)", v, __LINE__);
		if (h->rb == R && h->L->rb == R && sw) {
			h = rotR(h);
			______________________________I("./fig/", RBhead, h, "inserting %d (ln%d)", v, __LINE__);
		}
		if (h->L->rb == R && h->L->L->rb == R) {
			h = rotR(h);
			h->rb = B;
			h->R->rb = R;
			______________________________I("./fig/", RBhead, h, "inserting %d (ln%d)", v, __LINE__);
		}
		______________________________I("./fig/", RBhead, h, "inserting %d (ln%d)", v, __LINE__);
	} else {
		______________________________I("./fig/", RBhead, h, "inserting %d (ln%d)", v, __LINE__);
		h->R = RBinsert(h->R, item, 1);
		h->R->P = h;
		______________________________I("./fig/", RBhead, h, "inserting %d (ln%d)", v, __LINE__);
		if (h->rb == R && h->R->rb == R && !sw) {
			h = rotL(h);
			______________________________I("./fig/", RBhead, h, "inserting %d (ln%d)", v, __LINE__);
		}
		if (h->R->rb == R && h->R->R->rb == R) {
			h = rotL(h);
			h->rb = B;
			h->L->rb = R;
			______________________________I("./fig/", RBhead, h, "inserting %d (ln%d)", v, __LINE__);
		}
		______________________________I("./fig/", RBhead, h, "inserting %d (ln%d)", v, __LINE__);
	}
	//fixN(h);
	return h;
}
void Sedgewick_rb_insert(NODE * head, int item)
{
	RBhead = head;
	root = RBinsert(root, item, 0);
	root->rb = B;
}

#define hL		(h->L)
#define hR		(h->R)
#define hLL		(h->L->L)
#define hLR		(h->L->R)
#define hRL		(h->R->L)
#define hRR		(h->R->R)
#define chg_rb(x)	do{(x)->rb = ((x)->rb == R) ? B : R;}while(0);
void flip(NODE * h)
{
	assert(h != nil && hL != nil && hR != nil);
	assert(h->rb == R);

	/* h's left  child must be a 2-node */
	assert(hL->rb == B && hLL->rb == B && hLR->rb == B);

	/* h's right child must be a 2-node */
	assert(hR->rb == B && hRL->rb == B && hRR->rb == B);

	h->rb = B;
	hL->rb = R;
	hR->rb = R;
}
NODE * RBdelete(NODE * h, int v)
{
	assert(h != nil);

	______________________________D("./fig/", RBhead, h, "deleting %d (ln%d)", v, __LINE__);
	if (h == root && hL->rb == B && hR->rb == B)
		h->rb = R;

	if (h->val == v) {
		if (hL == nil || hR == nil) {
			NODE * x;
			if (hL != nil)
				x = hL;
			else
				x = hR;
			if (x != nil)
				x->P = h->P;
			if (h->P == nil) {
				assert(h == root->L);
				______________________________D("./fig/", RBhead, h, "deleting %d (ln%d)", v, __LINE__);
				return nil;
			} else if (h == h->P->L)
				h->P->L = x;
			else
				h->P->R = x;

			if (h->rb == B) {
				assert(x->rb == R);
				x->rb = B;
			}
			______________________________D("./fig/", RBhead, h, "deleting %d (ln%d)", v, __LINE__);
			free_node(h);
			return x;
		} else {
			NODE * x;
			for (x = hR; x->L != nil; x = x->L) {}
			______________________________D("./fig/", RBhead, h, "deleting %d (ln%d)", v, __LINE__);
			h->val = x->val;
			v = x->val;
		}
	}

	/* It's possible to get here even if the ``v'' has been found:
	 *         when (h has two children) and (SUCCESSOR(h)->val == v).
	 * Therefore, we'd use (h->val <= v), not (h->val < v).
	 */
	if (h->val <= v) {
		assert(hR != nil);

		/* transform if the right child is a 2-node */
		if (hR->rb == B && hRL->rb == B && hRR->rb == B) {
			if (h->rb == B) {
				assert(hL->rb == R);
				h = rotR(h);
				chg_rb(h);
				chg_rb(hR);
				______________________________D("./fig/", RBhead, h, "deleting %d (ln%d)", v, __LINE__);
			} else {
				assert(h->rb == R);
				if (hL->rb == B && hLL->rb == B && hLR->rb == B) {
					flip(h);
					______________________________D("./fig/", RBhead, h, "deleting %d (ln%d)", v, __LINE__);
				} else {
					if (hLR->rb == B) {
						h = rotR(h);
						chg_rb(h);
						chg_rb(hL);
						chg_rb(hR);
						chg_rb(hRR);
					} else {
						hL= rotL(hL);
						/* chg_rb(hL); */
						/* chg_rb(hLL); */
						______________________________D("./fig/", RBhead, h, "deleting %d (ln%d)", v, __LINE__);
						h = rotR(h);
						/* chg_rb(h); */
						/* chg_rb(hL); */
						chg_rb(hR);
						chg_rb(hRR);
					}
					______________________________D("./fig/", RBhead, h, "deleting %d (ln%d)", v, __LINE__);
				}
			}
		}
		assert(hR->rb == R || hRL->rb == R || hRR->rb == R);

		hR = RBdelete(hR, v);
		______________________________D("./fig/", RBhead, h, "deleting %d (ln%d)", v, __LINE__);
	} else if (v < h->val) {
		assert(hL != nil);
		/* transform if the left child is a 2-node */
		if (hL->rb == B && hLL->rb == B && hLR->rb == B) {
			if (h->rb == B) {
				assert(hR->rb == R);
				h = rotL(h);
				chg_rb(h);
				chg_rb(hL);
				______________________________D("./fig/", RBhead, h, "deleting %d (ln%d)", v, __LINE__);
			} else {
				assert(h->rb == R);
				if (hR->rb == B && hRL->rb == B && hRR->rb == B) {
					flip(h);
					______________________________D("./fig/", RBhead, h, "deleting %d (ln%d)", v, __LINE__);
				} else {
					if (hRL->rb == B) {
						h = rotL(h);
						chg_rb(h);
						chg_rb(hR);
						chg_rb(hL);
						chg_rb(hLL);
					} else {
						hR= rotR(hR);
						______________________________D("./fig/", RBhead, h, "deleting %d (ln%d)", v, __LINE__);
						h = rotL(h);
						chg_rb(hL);
						chg_rb(hLL);
					}
					______________________________D("./fig/", RBhead, h, "deleting %d (ln%d)", v, __LINE__);
				}
			}
		}
		/* the left child is no longer a 2-node */
		assert(!(hL->rb == B && hLL->rb == B && hLR->rb == B));

		hL = RBdelete(hL, v);
		______________________________D("./fig/", RBhead, h, "deleting %d (ln%d)", v, __LINE__);
	}
	return h;
}
void Sedgewick_rb_delete(NODE * head, int v)
{
	RBhead = head;
	assert(root->rb == B);
	root = RBdelete(root, v);
	root->rb = B;
	______________________________D("./fig/", RBhead, root, "deleting %d (ln%d)", v, __LINE__);
}

/*
 * implementation of Red-Black Tree Algorithm @ CLRS
 *
 * author: Forrest Y. Yu <forrest.yu@gmail.com>, http://forrestyu.net/
 *
 * @see Introduction To Algorithms, Chapter 13
 */

#include <stdio.h>
#include <assert.h>

#include "colorful.h"
#include "rbtlib.h"

/* 
 * RB-INSERT-FIXUP(T, z)
 *  1 while color[p[z]] = RED
 *  2     do if p[z] = left[p[p[z]]]
 *  3           then y ← right[p[p[z]]]
 *  4                if color[y] = RED
 *  5                   then color[p[z]] ← BLACK         ▹ Case 1
 *  6                        color[y] ← BLACK            ▹ Case 1
 *  7                        color[p[p[z]]] ← RED        ▹ Case 1
 *  8                        z ← p[p[z]]                 ▹ Case 1
 *  9                   else if z = right[p[z]]
 * 10                           then z ← p[z]            ▹ Case 2
 * 11                                LEFT-ROTATE(T, z)    ▹ Case 2
 * 12                           color[p[z]] ← BLACK      ▹ Case 3
 * 13                           color[p[p[z]]] ← RED     ▹ Case 3
 * 14                           RIGHT-ROTATE(T, p[p[z]])  ▹ Case 3
 * 15           else (same as then clause
 *                          with "right" and "left" exchanged)
 * 16 color[root[T]] ← BLACK
 */
void rb_insert_fixup(NODE * root, NODE * z)
{
	/* Legend
	 * ------
	 *     [x]  :  x is Red
	 *      x   :  x is Black
	 */
	NODE * y;
	while (z->P->rb == R) {
		if (z->P == z->P->P->L) {
			y = z->P->P->R;
			/*        #             #
			 *       / \           / \ 
			 *     [#]  y   or   [#]  y
			 *     /               \
			 *   [z]               [z]
			 */
			if (y->rb == R) {
				/*        #              #
				 *       / \            / \ 
				 *     [#] [y]   or   [#] [y]
				 *     /                \
				 *   [z]                [z]
				 */
				z->P->rb = B;
				y->rb = B;
				z->P->P->rb = R;
				z = z->P->P;
				/*       [z]            [z]
				 *       / \            / \ 
				 *      #   y    or    #   y
				 *     /                \
				 *   [#]                [#]
				 */
				______________________________I("./fig/", root, root, "fixup ln%d", __LINE__);
			} else {
				if (z == z->P->R) {
					/*      #
					 *     / \
					 *   [#]  y
					 *     \
					 *     [z]
					 */
					z = z->P;
					/*      #
					 *     / \
					 *   [z]  y
					 *     \
					 *     [#]
					 */
					rotate_left(root, z);
				}
				______________________________I("./fig/", root, root, "fixup ln%d", __LINE__);
				/*        #
				 *       / \
				 *     [#]  y
				 *     /
				 *   [z]
				 */
				z->P->rb = B;
				z->P->P->rb = R;
				______________________________I("./fig/", root, root, "fixup ln%d", __LINE__);
				/*       [#]  
				 *       / \ 
				 *      #   y
				 *     /
				 *   [z]
				 */
				rotate_right(root, z->P->P);
				/*    #
				 *   / \
				 * [z] [#]
				 *       \
				 *        y
				 */
				______________________________I("./fig/", root, root, "fixup ln%d", __LINE__);
			}
		} else {
			y = z->P->P->L;
			/*        #             #
			 *       / \           / \
			 *      y  [#]   or   y  [#]
			 *         /               \
			 *       [z]               [z]
			 */
			if (y->rb == R) {
				/*        #             #
				 *       / \           / \ 
				 *     [y] [#]   or  [y] [#]
				 *         /               \
				 *       [z]               [z]
				 */
				z->P->rb = B;
				y->rb = B;
				z->P->P->rb = R;
				z = z->P->P;
				/*       [#]           [#]
				 *       / \           / \
				 *      y   #    or   y   #
				 *         /               \
				 *       [z]               [z]
				 */
				______________________________I("./fig/", root, root, "fixup ln%d", __LINE__);
			} else {
				if (z == z->P->L) {
					/*        #
					 *       / \
					 *      y  [#]
					 *         /
					 *       [z]
					 */
					z = z->P;
					/*        #
					 *       / \
					 *      y  [z]
					 *         /
					 *       [#]
					 */
					rotate_right(root, z);
				}
				______________________________I("./fig/", root, root, "fixup ln%d", __LINE__);
				/*        #
				 *       / \
				 *      y  [#]
				 *           \
				 *           [z]
				 */
				z->P->rb = B;
				z->P->P->rb = R;
				______________________________I("./fig/", root, root, "fixup ln%d", __LINE__);
				/*       [#]
				 *       / \
				 *      y   #
				 *           \
				 *           [z]
				 */
				rotate_left(root, z->P->P);
				/*          #
				 *         / \
				 *       [#] [z]
				 *       /
				 *      y
				 */
				______________________________I("./fig/", root, root, "fixup ln%d", __LINE__);
			}
		}
	}
	root->L->rb = B;
}

void rb_insert(NODE * root, NODE * z)
{
	assert(root && root != nil);
	assert(z != nil && z->rb == R);

	NODE * p = root->L;
	NODE * y = nil;
	while (p != nil) {
		y = p;
		if (z->val < p->val)
			p = p->L;
		else
			p = p->R;
	}
	z->P = y;
	if (y == nil) {
		assert(root->L == nil);
		root->L = z;
	} else {
		if (z->val < y->val)
			y->L = z;
		else
			y->R = z;
	}
	z->L = nil;
	z->R = nil;
	z->rb = R;
	______________________________I("./fig/", root, root, "before fixup ln%d", __LINE__);
	rb_insert_fixup(root, z);
	______________________________I("./fig/", root, root, " after fixup ln%d", __LINE__);
}

/*
 * RB-DELETE-FIXUP(T, x)
 *  1 while x ≠ root[T] and color[x] = BLACK
 *  2     do if x = left[p[x]]
 *  3           then w ← right[p[x]]
 *  4                if color[w] = RED
 *  5                   then color[w] ← BLACK                        ▹  Case 1
 *  6                        color[p[x]] ← RED                       ▹  Case 1
 *  7                        LEFT-ROTATE(T, p[x])                     ▹  Case 1
 *  8                        w ← right[p[x]]                         ▹  Case 1
 *  9                if color[left[w]] = BLACK and color[right[w]] = BLACK
 * 10                   then color[w] ← RED                          ▹  Case 2
 * 11                        x p[x]                                   ▹  Case 2
 * 12                   else if color[right[w]] = BLACK
 * 13                           then color[left[w]] ← BLACK          ▹  Case 3
 * 14                                color[w] ← RED                  ▹  Case 3
 * 15                                RIGHT-ROTATE(T, w)               ▹  Case 3
 * 16                                w ← right[p[x]]                 ▹  Case 3
 * 17                         color[w] ← color[p[x]]                 ▹  Case 4
 * 18                         color[p[x]] ← BLACK                    ▹  Case 4
 * 19                         color[right[w]] ← BLACK                ▹  Case 4
 * 20                         LEFT-ROTATE(T, p[x])                    ▹  Case 4
 * 21                         x ← root[T]                            ▹  Case 4
 * 22        else (same as then clause with "right" and "left" exchanged)
 * 23 color[x] ← BLACK
 */
void rb_delete_fixup(NODE * root, NODE * x)
{
	NODE * w;
	while (x->P != nil && x->rb == B) {
		______________________________D("./fig/", root, x, "delete -- case 1 -- x:%d, w:%d (ln%d)", x->val, w->val, __LINE__);
		if (x == x->P->L) {
			/*        #?
			 *       / \
			 *      x   w?
			 */
			w = x->P->R;
			if (w->rb == R) {
				/************************
				 *        Case 1        *
				 ************************/
				assert(x->P->rb == B);
				/*        #            ▹ w is red, so its parent is black
				 *       / \
				 *      x  [w]
				 *         / \
				 *        @   %        ▹ w is red, so the children are black
				 */
				______________________________D("./fig/", root, x, "delete -- case 1 -- x:%d, w:%d (ln%d)", x->val, w->val, __LINE__);
				w->rb = B;
				x->P->rb = R;
				rotate_left(root, x->P);
				/*          w
				 *         / \
				 *       [#]  %
				 *       / \
				 *      x   @          ▹ note: inorder (x#@w%) unchanged
				 */
				w = x->P->R;
				/*          #
				 *         / \
				 *       [#]  %
				 *       / \
				 *      x   w          ▹ w, sibling of x, is black now, Case 1 has been converted to Case 2
				 */
				______________________________D("./fig/", root, x, "delete -- case 1 -- x:%d, w:%d (ln%d)", x->val, w->val, __LINE__);
			}
			/*        #?
			 *       / \
			 *      x   w                  ▹ w is black
			 */
			if (w->L->rb == B && w->R->rb == B) {
				/************************
				 *        Case 2        *
				 ************************/
				/*        #?
				 *       / \
				 *      x   w
				 *         / \
				 *        @   %
				 */
				______________________________D("./fig/", root, x, "delete -- case 2 -- x:%d, w:%d (ln%d)", x->val, w->val, __LINE__);
				w->rb = R;
				x = x->P;
				/*        x?
				 *       / \
				 *      #  [w]
				 *         / \
				 *        @   %        ▹ to redden a black node w makes x a balanced subtree
				 */
				______________________________D("./fig/", root, x, "delete -- case 2 -- x:%d, w:%d (ln%d)", x->val, w->val, __LINE__);
			} else {
				/*        #?              #?               #?
				 *       / \	         / \   	          / \
				 *      x   w     or    x   w      or    x   w
				 *         / \	           / \ 	            / \
				 *       [@]  %	          @  [%]          [@] [%]
				 */
				if (w->R->rb == B) {
					/************************
					 *        Case 3        *
					 ************************/
					/*        #?
					 *       / \
					 *      x   w
					 *         / \
					 *       [@]  %
					 */
					______________________________D("./fig/", root, x, "delete -- case 3 -- x:%d, w:%d (ln%d)", x->val, w->val, __LINE__);
					w->L->rb = B;
					w->rb = R;
					rotate_right(root, w);
					/*        #?
					 *       / \
					 *      x   @
					 *         / \
					 *           [w]
					 *           / \
					 *              %
					 */
					w = x->P->R;
					/*        #?
					 *       / \
					 *      #   w
					 *         / \
					 *           [#]       ▹ Case 3 is converted to Case 4
					 *           / \
					 *              %
					 */
					______________________________D("./fig/", root, x, "delete -- case 3 -- x:%d, w:%d (ln%d)", x->val, w->val, __LINE__);
				}
				/************************
				 *        Case 4        *
				 ************************/
				/*         #?                  let
				 *       /    \                    c = (color[#]==Black ? 1 : 0)
				 *      x      w                   c'= (color[@]==Black ? 1 : 0),
				 *     / \   /   \             how many black nodes there are from # to a leaf:
				 *    α β @?   [%]           α         β         γ         δ         ε         ζ
				 *         / \   / \           1+c        1+c        1+c'+c     1+c'+c     1+c        1+c
				 *        γ δ ε ζ
				 */
				______________________________D("./fig/", root, x, "delete -- case 4 -- x:%d, w:%d (ln%d)", x->val, w->val, __LINE__);
				w->rb = x->P->rb;
				x->P->rb = B;
				w->R->rb = B;
				rotate_left(root, x->P);
				/*             w?
				 *          /     \
				 *         #       %
				 *       /   \    / \          how many black nodes there are from w to a leaf:
				 *      x     @? ε ζ         α         β         γ         δ         ε         ζ
				 *     / \   / \               2+c        2+c        1+c'+c     1+c'+c     1+c        1+c
				 *    α β γ δ              therefore α and β restore their original black-depth and we're done
				 */
				______________________________D("./fig/", root, x, "delete -- case 4 -- x:%d, w:%d (ln%d)", x->val, w->val, __LINE__);
				x = root->L;
			}
		} else {	/* x == x->P-> R */
			/*        #
			 *       / \
			 *      w   x
			 */
			w = x->P->L;
			if (w->rb == R) {
				______________________________D("./fig/", root, x, "delete -- case 1 -- x:%d, w:%d (ln%d)", x->val, w->val, __LINE__);
				w->rb = B;
				x->P->rb = R;
				rotate_right(root, x->P);
				w = x->P->L;
				______________________________D("./fig/", root, x, "delete -- case 1 -- x:%d, w:%d (ln%d)", x->val, w->val, __LINE__);
			}
			if (w->R->rb == B && w->L->rb == B) {
				______________________________D("./fig/", root, x, "delete -- case 2 -- x:%d, w:%d (ln%d)", x->val, w->val, __LINE__);
				w->rb = R;
				x = x->P;
				______________________________D("./fig/", root, x, "delete -- case 2 -- x:%d, w:%d (ln%d)", x->val, w->val, __LINE__);
			} else {
				if (w->L->rb == B) {
					______________________________D("./fig/", root, x, "delete -- case 3 -- x:%d, w:%d (ln%d)", x->val, w->val, __LINE__);
					w->R->rb = B;
					w->rb = R;
					rotate_left(root, w);
					w = x->P->L;
					______________________________D("./fig/", root, x, "delete -- case 3 -- x:%d, w:%d (ln%d)", x->val, w->val, __LINE__);
				}
				______________________________D("./fig/", root, x, "delete -- case 4 -- x:%d, w:%d (ln%d)", x->val, w->val, __LINE__);
				w->rb = x->P->rb;
				x->P->rb = B;
				w->L->rb = B;
				rotate_right(root, x->P);
				x = root->L;
				______________________________D("./fig/", root, x, "delete -- case 4 -- x:%d, w:%d (ln%d)", x->val, w->val, __LINE__);
			}
		}
	}
	x->rb = B;
}

NODE * rb_delete(NODE * root, NODE * z)
{
	NODE * y = z;
	if (z->L != nil && z->R != nil)
		for (y = z->R; y->L != nil; y = y->L) {}
	NODE * x;
	if (y->L != nil)
		x = y->L;
	else
		x = y->R;

	/* at this time, x might be nil
	 * if so, this is the only chance that nil->P is used
	 */
	x->P = y->P;

	if (y->P == nil) {	/* root */
		assert(y == root->L);
		root->L = x;
	} else if (y == y->P->L) {
		y->P->L = x;
	} else {
		y->P->R = x;
	}
	if (y != z)
		z->val = y->val;
	if (y->rb == B) {
		/* printf("%sy:%d, x:%d, x->P:%d%s\n", RED, y->val, x->val, x->P->val, NOCOLOR); */
		rb_delete_fixup(root, x);
	}
	return y;
}

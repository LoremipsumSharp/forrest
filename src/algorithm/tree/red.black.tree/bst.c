/*
 * BST helper routines
 *
 * author: Forrest Y. Yu <forrest.yu@gmail.com>, http://forrestyu.net/
 */

#include <stdio.h>
#include <assert.h>

#include "colorful.h"
#include "rbtlib.h"

/* returns 1 if success, otherwise 0 */
NODE * rotate_left(NODE * root, NODE * x)
{
	NODE * y = x->R;

	if (y != nil) {
		x->R = y->L;
		if (x->R != nil)
			x->R->P = x;

		y->P = x->P;
		if (x->P == nil) {
			root->L = y;
		} else {
			if (x->P->L == x)
				x->P->L = y;
			else
				x->P->R = y;
		}

		y->L = x;
		x->P = y;
	}

	return y;
}

NODE * rotate_right(NODE * root, NODE * y)
{
	NODE * x = y->L;

	if (x != nil) {
		y->L = x->R;
		if (y->L != nil)
			y->L->P = y;

		x->P = y->P;
		if (y->P == nil) {
			root->L = x;
		} else {
			if (y->P->L == y)
				y->P->L = x;
			else
				y->P->R = x;
		}

		x->R = y;
		y->P = x;
	}

	return x;
}

NODE * rb_search(NODE * T, int v)
{
	if (T != nil && T->L != nil && T->L->P == nil) /* the root */
		rb_search(T->L, v);

	if (T == nil)
		return 0;

	if (v < T->val) {
		return rb_search(T->L, v);
	} else if (v > T->val) {
		return rb_search(T->R, v);
	} else {
		return T;
	}
}

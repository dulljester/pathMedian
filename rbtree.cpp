/*
 * Red-Black Tree implementation for maintaining
 * Order Statistics Set
 * Fully-functional RB-Tree
 */
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#define MAXLOG (21)
#define N (1<<MAXLOG)
enum { RED, BLACK };
enum { L, R };
using namespace std;

class RBtree {
private:
	typedef struct cell {
		struct cell *son[2],*p;
		char color;
		int weight, freq, card;
	} cell;
#define flip_color(x) ((x)->color ^= 1)
#define color(x) ((x)->color)
#define which_son(x) ((x)->p->son[L]==x?L:R)
	cell pool[N],*ptr,*root,*NIL;
	cell *init_cell() {
		cell *x = ptr++;
		x->son[L] = x->son[R] = x->p = NIL, x->weight = x->freq = x->card = 0, x->color = RED;
		return x;
	}
	void rotate( cell *x, int i ) {
		cell *y;
		assert( (y = x->son[i^1]) != NIL );
		if ( (x->son[i^1] = y->son[i]) != NIL )
			y->son[i]->p = x;
		y->son[i] = x;
		if ( (y->p = x->p) != NIL )
			x->p->which_son[x] = y;
		for ( x->p = y; x != NIL; x->card = x->freq+x->son[L]->card+x->son[R]->card, x = x->p ) ;
	}	
	void fixup( cell *x ) {
	}
public:
	void init() {
		ptr = pool, NIL = init_cell();
		NIL = ptr++, NIL->p=NIL->son[L]=NIL->son[R]=NIL,NIL->color=BLACK, root = init_cell();
	}
	void push( const int entry ) {
		cell *x,*y,**hold,*z,*g;
		for ( hold = &root, y = NIL, x = root;;) {
			if ( x == NIL ) {
				*hold = x = init_cell();
				x->p = y, x->freq = 1, x->weight = entry;
				for ( z = x->p; z != NIL; ++z->card, z = z->p ) ;
				break ;
			}
			if ( x->weight == entry ) {
				for ( ++x->freq, ++x->card, z = x->p; z != NIL; ++z->card, z = z->p ) ;
				break ;
			}
			if ( x->weight < entry )
				y = x, hold = &x->son[R], x = x->son[R];
			else y = x, hold = &x->son[L], x = x->son[L];
		}
		while ( x != root && color(x->p) == RED ) {
			g = x->p->p, i = which_son(x->p), y = g->son[i^1];
			if ( color(y) == RED ) {
				flip_color(x->p), flip_color(y), flip_color(g), x = g;
				continue ;
			}
			flip_color(x->p), flip_color(g), rotate(g,i^1), x = x->p;
		}
		if ( x == root )
			x->color = BLACK;
	}
	cell *find( const int item ) const {
		cell *x;
		for ( x = root; x != NIL && x->weight != item; )
			if ( x->weight < item )
				x = x->son[R];
			else x = x->son[L];
		return x==NIL?NULL:x;
	}
	void erase( cell *z ) {
		cell *x,*y;
		int i;
		if ( --z->freq ) {
			for ( x = z; x != NIL; --x->card, x = x->p ) ;
			return ;
		}
		assert( !z->freq ) ;
		if ( (y = (z->son[R] != NIL?z->son[R]:z)) != z ) {
			for ( ;y->son[L] != NIL; y = y->son[L] ) ;
		}
	}
} *T;

int main() {
	int i,j,k;
	return 0;
}


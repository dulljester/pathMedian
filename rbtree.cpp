/*
 * Red-Black Tree implementation for maintaining
 * Order Statistics Set
 * Fully-functional RB-Tree
 *
 * Offline algorithm for Path Median Selection
 */
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#define MAXLOG (21)
#define N (1<<MAXLOG)
#define MAXE (N<<1)
#define oo (1<<30)
#define NONE (-1)
enum { RED, BLACK };
enum { L, R };
using namespace std;

class RBtree {
private:
	typedef struct cell {
		struct cell *son[2],*p;
		char c;
		int weight, freq, card;
	} cell;
#define flip(x) ((x)->c ^= 1)
#define color(x) ((x)->c)
#define which_son(x) ((x)->p->son[L]==x?L:R)
	cell pool[N],*ptr,*root,*NIL;
	cell *init_cell() {
		cell *x = ptr++;
		x->son[L] = x->son[R] = x->p = NIL, x->weight = x->freq = x->card = 0, x->c = RED;
		return x;
	}
	void update_upwards( cell *x ) {
		for ( ;x != NIL; x->card = x->freq+x->son[L]->card+x->son[R]->card, x = x->p ) ;
	}
	void rotate( cell *x, int i ) {
		cell *y;
		assert( (y = x->son[i^1]) != NIL );
		if ( (x->son[i^1] = y->son[i]) != NIL )
			y->son[i]->p = x;
		y->son[i] = x;
		if ( (y->p = x->p) != NIL )
			x->p->son[which_son(x)] = y;
		else root = y;
		x->p = y, update_upwards(x);
	}	
	void fixup( cell *x ) {
		int i;
		cell *y,*p;
		while ( x != root && color(x) == BLACK ) {
			i = which_son(x), y = x->p->son[i^1];
			if ( color(y) == RED ) {
				flip(x->p), flip(y), rotate(x->p,i);
				continue ;
			}
			if ( color(y->son[i^1]) == RED ) {
				y->c=x->p->c,x->p->c=BLACK, rotate(x->p,i), x = root;
				break ;
			}
			if ( color(y->son[i]) == RED ) {
				flip(y->son[i]), flip(y), rotate(y,i^1);
				continue ;
			}
			if ( color(x->p) == RED ) {
				flip(y), flip(x->p), x = root;
				break ;
			}
			flip(y), x = x->p;
		}
		x->c=BLACK;
	}
	int kth_weight( cell *x, int k ) const {
		if ( x->son[L] == NIL && x->son[R] == NIL )
			return x->weight;
		if ( x->son[L] != NIL && x->son[L]->card > k )
			return kth_weight(x->son[L],k);
		return kth_weight(x->son[R],k-x->son[L]->card);
	}
public:
	void init() {
		ptr = pool, NIL = init_cell();
		NIL->p=NIL->son[L]=NIL->son[R]=NIL,NIL->c=BLACK, root = NIL;
	}
	int kth_weight( int k ) const {
		return root->card > k ? kth_weight(root,k) : +oo;
	}
	void push( const int entry ) {
		cell *x,*y,**hold,*z,*g;
		int i;
		for ( hold=&root, y=(x=root)->p;;) {
			if ( x == NIL ) {
				*hold = x = init_cell();
				x->p = y, x->freq = 1, x->weight = entry;
				update_upwards(x);
				break ;
			}
			if ( x->weight == entry ) {
				update_upwards(x);
				break ;
			}
			if ( x->weight < entry )
				y = x, hold = &x->son[R], x = x->son[R];
			else y = x, hold = &x->son[L], x = x->son[L];
		}
		while ( x != root && color(x->p) == RED ) {
			g = x->p->p, i = which_son(x->p), y = g->son[i^1];
			if ( color(y) == RED ) {
				flip(x->p), flip(y), flip(g), x = g;
				continue ;
			}
			if ( which_son(x) != i ) {
				x = x->p, rotate(x,i);
				continue ;
			}
			flip(x->p), flip(g), rotate(g,i^1);
		}
		root->c = BLACK;
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
		if ( !z || z == NIL ) return ;
		if ( --z->freq ) {
			update_upwards(z);
			return ;
		}
		assert( !z->freq ) ;
		if ( z->son[L] == NIL || z->son[R] == NIL )
			y = z;
		else y = z->son[R];
		if ( y == z->son[R] ) {
			for (;y->son[L] != NIL; y = y->son[L] ) ;
			z->weight = y->weight, z->freq = y->freq;
		}
		assert( y->son[L] == NIL || y->son[R] == NIL );
		x=(y->son[L]==NIL?y->son[R]:y->son[L]);
		if ( (x->p = y->p) == NIL )
			root = x;
		else y->p->son[which_son(y)] = x;
		update_upwards(x);
		if ( color(y) == BLACK )
			fixup(x);
	}
	RBtree() { init(); }
} *T;

class Graph {
private:
	int last[N],next[MAXE],to[MAXE],p[N],E,sigma,weight[N],
		anc[N][MAXLOG],K,n,d[N],seen[N],yes;
	void add_arcs( int x, int y ) {
		int i = E++, j = E++;
		to[i] = y, next[i] = last[x], last[x] = i;
		to[j] = x, next[j] = last[y], last[y] = j;
	}
	int up( int x, unsigned int u ) const {
		for ( int k = 0; u; ++k, u >>= 1 )
			if ( u & 1 ) x = anc[x][k];
		return x;
	}
	void dfs( int x, int depth ) {
		int i,y;
		assert( seen[x] != yes );
		for ( seen[x] = yes, d[x] = depth, i = last[x]; i != NONE; i = next[i] )
			if ( seen[y = to[i]] != yes ) {
				for ( p[y] = i, anc[y][0] = x, k = 1; anc[y][k-1] != NONE; anc[y][k] = anc[anc[y][k-1]][k-1], ++k ) ;
				dfs(y,1+depth);
			}
	}
public:
	bool read_graph() {
		int i,j,k;
		if ( 2 != scanf("%d %d",&n,&sigma) )
			return false ;
		for ( E = 0, i = 0; i < n; last[i++] = NONE ) ;
		for ( i = 0; i < n; scanf("%d",&weight[i++]) ) ;
		for ( k = 0; k < n-1; ++k )
			scanf("%d %d",&i,&j), add_arcs(i,j);
		for ( K = 0; (1<<K) <= n; ++K ) ;
		for ( yes = 0, i = 0; i < n; ++i )
			for ( seen[i] = 0, k = 0; k < K; anc[i][k++] = NONE ) ;
		return true ;
	}
	void preprocess() {
		++yes, dfs(0,0);
	}
	int lca( int x, int y ) const {
		if ( d[x] > d[y] )
			return lca(up(x,d[x]-d[y]),y);
		if ( d[x] < d[y] )
			return lca(y,x);
		if ( x == y )
			return x;
		for ( int k = K-1; k; --k ) {
			assert( anc[x][k] == anc[y][k] );
			if ( anc[x][k-1] != anc[y][k-1] )
				x = anc[x][k-1], y = anc[y][k-1];
		}
		return anc[x][0];
	}
} G;

int main() {
	int i,j,k,qr;
	for ( ;G.read_graph(); ) {
		G.preprocess();
	}
	return 0;
}


/*
 * Red-Black Tree implementation for maintaining
 * Order Statistics Set
 * Fully-functional RB-Tree
 *
 * Offline algorithm for Path Median Selection
 */
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <chrono>
#define MAXLOG (21)
#define N (1<<MAXLOG)
#define MAXE (N<<1)
#define oo (1<<30)
#define NONE (-1)
#include <map>
enum { RED, BLACK };
enum { L, R };
using namespace std;
#define USE_STL 0

class RBtree {
private:
#if USE_STL
	map<int,int> s;
#endif
	typedef struct cell {
		struct cell *son[2],*p;
		char c;
		int weight, freq, card;
	} cell;
#define flip(x) ((x)->c ^= 1)
#define color(x) ((x)->c)
#define which_son(x) ((x)->p->son[L]==x?L:R)
	int n;
#if USE_STL
	int m;
#endif
	cell *root,*NIL;
	cell *init_cell() {
		cell *x = (cell *)malloc(sizeof *x);
		x->son[L]=x->son[R]=x->p=NIL,x->weight=+oo,x->freq=x->card=0,x->c=RED;
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
	void destroy( cell *x ) {
		if ( x != NIL ) destroy(x->son[L]), destroy(x->son[R]), free(x);
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
				y->c=x->p->c,x->p->c=BLACK,flip(y->son[i^1]), rotate(x->p,i), x = root;
				continue ;
			}
			if ( color(y->son[i]) == RED ) {
				flip(y->son[i]), flip(y), rotate(y,i^1);
				continue ;
			}
			if ( color(x->p) == RED ) {
				flip(y), flip(x->p), x = root;
				continue ;
			}
			flip(y), x = x->p;
		}
		x->c = BLACK;
	}
	int kth_weight( cell *x, int k ) const {
		if ( x->son[L] == NIL && x->son[R] == NIL ) {
			assert( x->freq > k );
			return x->weight;
		}
		if ( x->son[L]->card > k )
			return kth_weight(x->son[L],k);
		if ( x->son[L]->card+x->freq <= k )
			return kth_weight(x->son[R],k-x->son[L]->card-x->freq);
		return x->weight;
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
		//if ( !z || z == NIL ) return ;
		assert( z && z != NULL );
		--n;
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
		update_upwards(y->p);
		if ( color(y) == BLACK )
			fixup(x);
		free(y);
		assert( size() == root->card );
	}
public:
	int inline size() const { return n; }
	void init() {
#if USE_STL
		m = 0, s.clear();
#else
		n = 0, NIL = init_cell();
		NIL->p=NIL->son[L]=NIL->son[R]=NIL,NIL->c=BLACK,root=NIL;
#endif
	}
	int kth_weight( int k ) const {
#if USE_STL
		int ax = 0;
		for ( map<int,int> :: const_iterator it = s.begin(); it != s.end(); ++it ) {
			assert( it->second > 0 ) ;
			if ( it->second+ax <= k ) {
				ax += it->second;
				continue ;
			}
			assert( ax <= k && ax+it->second > k );
			return it->first;
		}
		assert(0);
#else
		return root->card>k?kth_weight(root,k):+oo; 
#endif
	}
	void push( const int entry ) {
#if USE_STL
		if ( s.count(entry) )
			++s[entry];
		else s[entry] = 1;
		++m;
#else
		cell *x,*y,**hold,*z,*g;
		int i;
		for ( ++n, hold=&root, y=(x=root)->p;;) {
			if ( x == NIL ) {
				*hold = x = init_cell();
				x->p = y, x->freq = 1, x->weight = entry;
				update_upwards(x);
				break ;
			}
			if ( x->weight == entry ) {
				++x->freq, update_upwards(x);
				return ;
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
		if ( size() != root->card )
			printf("This %d %d\n",size(),root->card);
		assert( size() == root->card );
#endif
	}
	RBtree() { init(); }
	~RBtree() { destroy(root); }
	void erase( const int w ) { 
#if USE_STL
		if ( !--s[w] ) s.erase(w);
		--m;
#else
		erase(find(w));
#endif
	}
	int get_median() const {
#if USE_STL
		return kth_weight(m>>1);
#else
		return kth_weight(root->card>>1);
#endif
	}
} *T;

class Sequence {
private:
	int first[N], last[N], m, c[N];
	int inline int_at( int idx ) const { return c[idx]; }
public:
	void init() { m = 0; }
	void inline add_first( int x ) { first[x] = m, c[m++] = x; }
	void inline add_last( int x ) { last[x] = m, c[m++] = x; }
	int inline get_first( int x ) const { return first[x]; }
	int inline get_last( int x ) const { return last[x]; }
	int inline size() const { return m; }
	int operator[] ( const int i ) const { return int_at(i); }
} s;

class Graph {
private:
	int last[N],next[MAXE],to[MAXE],p[N],E,sigma,weight[N],
		anc[N][MAXLOG+1],K,n,d[N],seen[N],yes;
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
	void dfs( int x, int depth, Sequence &s ) {
		int i,y,k;
		assert( seen[x] != yes );
		for ( seen[x] = yes, s.add_first(x), d[x] = depth, i = last[x]; i != NONE; i = next[i] )
			if ( seen[y = to[i]] != yes ) {
				for ( p[y] = i, anc[y][0] = x, k = 1; anc[y][k-1] != NONE; anc[y][k] = anc[anc[y][k-1]][k-1], ++k ) ;
				dfs(y,1+depth,s);
			}
		s.add_last(x);
	}
	int inline get_weight( int x ) const { return weight[x]; }
public:
	inline int get_sigma() const { return sigma; }
	int operator [] ( const int x ) const { return get_weight(x); }
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
	void preprocess( Sequence &s ) { s.init(), ++yes, dfs(0,0,s); }
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
	/* Pre-condition: p is an ancestor of x
	 * Post-condition: returns vertex "v" which is the first vertex on the path from p to x, after p
	 */
	int pre_ancestor( int x, int p ) const {
		assert( p != x );
		return up(x,d[x]-d[p]-1);
	}
	int inline size() const { return n; }
} G;

int B;

struct st_query {
	int left, right, idx, ans, lca;
	bool operator < ( const st_query &rhs ) const {
		if ( left/B == rhs.left/B )
			return right < rhs.right;
		return left/B < rhs.left/B;
	}
	st_query( int l, int r, int LCA, int i ) : left(l), right(r), lca(LCA), idx(i) {}
	void set_answer( int weight ) { ans = weight; }
} ;

struct comparator {
	bool operator () ( const st_query &a, const st_query &b ) {
		return a.idx < b.idx;
	}
};

vector<st_query> queries;
char vcnt[N];

int main( int argc, char **argv ) {
	int oqr,i,j,k,qr,t,left,right,x,y,nleft,nright,lca,cs = 0;
	double ax = 0;
	FILE *fp = fopen(argv[1],"w");
	for ( ;G.read_graph() && ++cs; ) {
		G.preprocess(s);
		auto start = std::chrono::high_resolution_clock::now();
		for ( B = 1; B <= s.size()/B; ++B ) ;
		for ( t = 0, queries.clear(), scanf("%d",&qr), oqr = qr; qr-- && 2 == scanf("%d %d",&i,&j); ++t ) {
			assert( i != j );
			k = G.lca(i,j);
			if ( k != i && k != j ) {
				if ( s.get_last(i) < s.get_first(j) )
					queries.push_back(st_query(s.get_last(i),s.get_first(j),k,t));
				else 
					queries.push_back(st_query(s.get_last(j),s.get_first(i),k,t));
				continue ;
			}
			if ( k == i ) {
				assert( s.get_first(i) < s.get_first(j) );
				queries.push_back(st_query(s.get_first(i),s.get_first(j),k,t));
				continue ;
			}
			assert( s.get_first(j) < s.get_first(i) );
			queries.push_back(st_query(s.get_first(j),s.get_first(i),k,t));
		}
		sort(queries.begin(),queries.end());
		for ( T = new RBtree(), i = 0; i < G.size(); vcnt[i++] = 0 ) ;
		left = queries[0].left, right = queries[0].left-1;
		for ( t = 0; t < (int)queries.size(); left = nleft, right = nright, ++t ) {
			nleft = queries[t].left, nright = queries[t].right;
			for (;left < nleft; ) {
				if ( !--vcnt[x = s[left++]] )
					T->erase(G[x]);
				else if ( vcnt[x] == 1 )
					T->push(G[x]);
			}
			for ( ;right > nright; ) {
				if ( !--vcnt[x = s[right--]] )
					T->erase(G[x]);
				else if ( vcnt[x] == 1 )
					T->push(G[x]);
			}
			for ( ;left > nleft; ) {
				if ( 1 == ++vcnt[x = s[--left]] )
					T->push(G[x]);
				else if ( 2 == vcnt[x] )
					T->erase(G[x]);
			}
			for ( ;right < nright; ) {
				if ( 1 == ++vcnt[x = s[++right]] )
					T->push(G[x]);
				else if ( 2 == vcnt[x] )
					T->erase(G[x]);
			}
			if ( (lca = queries[t].lca) != s[left] && queries[t].lca != s[right] ) {
				assert( !vcnt[lca] );
				T->push(G[lca]);
				/*if ( ++vcnt[lca] == 1 )
					T.push(G[lca]);
				else if ( vcnt[lca] == 2 )
					T.erase(G[lca]);*/
			}
			queries[t].ans = T->get_median();
			if ( (lca = queries[t].lca) != s[left] && queries[t].lca != s[right] ) {
				assert( !vcnt[lca] );
				T->erase(G[lca]);
				/*
				if ( ++vcnt[lca] == 1 )
					T.push(G[lca]);
				else if ( vcnt[lca] == 2 )
					T.erase(G[lca]);*/
			}
		}
		sort(queries.begin(),queries.end(),comparator());
		for ( i = 0; i < (int)queries.size(); printf("%d\n",queries[i++].ans) ) ;
		auto finish = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = finish - start;
		ax += elapsed.count()/oqr;
		fprintf(fp,"%.6lf\n",elapsed.count()/oqr);
		delete T;
	}
	//printf("%lf\n",ax);
	fprintf(fp,"%.6lf\n",ax/cs);
	fclose(fp);
	return 0;
}


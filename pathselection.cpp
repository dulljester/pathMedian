/*
 */
#include <cassert>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <chrono>
#define N (1<<MAXLOG)
#define MAXE (N<<1)
#define oo (1<<30)
#define NONE (-1)
#define MAXLOG (21)
using namespace std;

/* _left[id] points to the place in array "chain" where the chain "id" starts;
* _right[id] points to the place in array "chain" where the chain "id" ends;
* pos[x] is the position of "x" in its chain
* which_chain[x] is self-explanatory
 */
int to[MAXE],_next[MAXE],last[N],E,n,weight[N],p[N],K,
	d[N],best_child[N],card[N],seen[N],yes,
	pos[N],chain_id,which_chain[N],chain[N],_left[N],_right[N],cur,
	anc[N][MAXLOG+1];

int up( int x, unsigned int u ) {
	for ( int k = 0; u; u >>= 1, ++k )
		if ( u&1 )
			x = anc[x][k];
	return x;
}

int lca( int x, int y, int *pre_lca_x, int *pre_lca_y ) {
	int k;
	if ( d[x] > d[y] )
		return lca(up(x,d[x]-d[y]),y,pre_lca_x,pre_lca_y);
	if ( d[x] < d[y] )
		return lca(x,up(y,d[y]-d[x]),pre_lca_x,pre_lca_y);
	assert( d[x] == d[y] );
	if ( x == y ) 
		return x;
	for ( k = K-1; k; --k ) {
		assert( anc[x][k] == anc[y][k] );
		if ( anc[x][k-1] != anc[y][k-1] ) 
			x = anc[x][k-1], y = anc[y][k-1];
	}
	*pre_lca_x = x, *pre_lca_y = y;
	assert( anc[x][0] == anc[y][0] );
	return anc[x][0];
}

void add_arcs( int x, int y ) {
	int i = E++, j = E++;
	to[i] = y, _next[i] = last[x], last[x] = i;
	to[j] = x, _next[j] = last[y], last[y] = j;
}

/* calculate subtree size + store the edge leading to the heaviest subtree */
int dfs( int x, int depth ) {
	int k,i,y;
	if ( seen[x] == yes )
		return card[x]; 
	for ( d[x] = depth, seen[x] = yes, best_child[x] = NONE, card[x] = 1, i = last[x]; i != NONE; i = _next[i] )
		if ( seen[y = to[i]] != yes ) {
			for ( p[y] = i, anc[y][0] = x, k = 1; anc[y][k-1] != NONE; anc[y][k] = anc[anc[y][k-1]][k-1], ++k ) ;
			card[x] += dfs(y,depth+1);
			if ( best_child[x] == NONE || dfs(y,depth+1) > card[to[best_child[x]]] )
				best_child[x] = i;
		}
	return card[x];
}

/* perform heavy-path-decomposition 
 * chain array has all the vertices listed in preorder,
 * and all the vertices on a heavy path lie contiguously */
void hld( int x, int edge_id ) {
	int i,y;

	if ( edge_id == NONE ) 
		_left[++chain_id] = cur;

	which_chain[chain[cur++]=x]=chain_id;
	_right[chain_id] = cur-1, pos[x] = _right[chain_id]-_left[chain_id];

	if ( (i = best_child[x]) != NONE )
		hld(to[i],i);

	for ( i = last[x]; i != NONE; i = _next[i] )
		if ( i != best_child[x] && p[y = to[i]] == i ) 
			hld(y,NONE);
}

class Wtree {
private:
	Wtree *_left,*_right;
	int n,*c,l,r;
	bool is_leaf;
	int map_left( int i ) const { return i < 0 ? -1:i-c[i]; }
	int map_right( int i ) const { return i < 0 ? -1:c[i]-1; }
	pair<int,int> left_segment( const pair<int,int> &p ) const {
		int j = map_left(p.second), i = map_left(p.first-1)+1;
		return make_pair(i,j);
	}
	pair<int,int> right_segment( const pair<int,int> &p ) const {
		int j = map_right(p.second), i = map_right(p.first-1)+1;
		return make_pair(i,j);
	}
	int left_cardinality( const pair<int,int> &p ) const {
		pair<int,int> q = left_segment(p);
		return max(0,q.second-q.first+1);
	}
	int right_cardinality( const pair<int,int> &p ) const {
		pair<int,int> q = right_segment(p);
		return max(0,q.second-q.first+1);
	}
public:
	Wtree( const int *A, int n, int l, int r ) {
		int mid = (l+r)>>1,i,j,k;
		_left = _right = NULL;
		if ( (this->is_leaf = (this->l = l) == (this->r = r)) || !(this->n = n) ) {
			this->n = n, c = NULL;
			return ;
		}
		c = (int *)calloc(this->n = n,sizeof *c);
		int *L,*R, ll = 0, lr = 0;
		for ( i = 0; i < n; ++i )
			if ( A[i] <= mid )
				++ll, c[i] = (i?c[i-1]:0);
			else ++lr, c[i] = (i?c[i-1]:0)+1;
		if ( ll )
			L = (int *)malloc(ll*sizeof *L);
		if ( lr )
			R = (int *)malloc(lr*sizeof *R);
		for ( j = k = 0, i = 0; i < n; ++i )
			if ( A[i] > mid )
				R[k++] = A[i];
			else L[j++] = A[i];
		assert( ll == j && lr == k );
		_left = ll?new Wtree(L,ll,l,mid):NULL, _right = lr?new Wtree(R,lr,mid+1,r):NULL;
		if ( _left ) free(L);
		if ( _right ) free(R);
	}
	int kth_weight( vector<pair<int,int> > &vec, int k ) const {
		assert( 0 <= k && k < n );
		assert( vec.size() );

		if ( is_leaf ) 
			return r;

		int ll = 0, rr = 0,i,j;
		for ( i = 0; i < (int)vec.size(); ++i )
			ll += left_cardinality(vec[i]), rr += right_cardinality(vec[i]);
		if ( ll > k ) {
			for ( i = 0; i < (int)vec.size(); ++i )
				vec[i] = left_segment(vec[i]);
			assert( _left );
			return _left->kth_weight(vec,k);
		}
		else {
			for ( i = 0; i < (int)vec.size(); ++i )
				vec[i] = right_segment(vec[i]);
			assert( _right );
			return _right->kth_weight(vec,k-ll);
		}
	}
} *wt;

int sigma;

bool read_preprocess() {
	int i,j,k;
	if ( 2 != scanf("%d %d",&n,&sigma) )
		return false ;
	for ( K = 0; (1<<K) <= n; ++K ) ;
	for ( i = 0; i < n; ++i )
		for ( k = 0; k < K; anc[i][k++] = NONE ) ;
	for ( E = 0, i = 0; i < n; last[i++] = NONE ) ;
	for ( i = 0; i < n; scanf("%d",&weight[i++]) ) ;
	for ( k = 0; k < n-1; ++k )
		scanf("%d %d",&i,&j), add_arcs(i,j);
	chain_id=NONE, ++yes, dfs(0,0), cur=0, hld(0,NONE);
	assert( cur == n );
	int *w = (int *)malloc(n*sizeof *w);
	for ( i = 0; i < cur; ++i )
		w[i] = weight[chain[i]];
	wt = new Wtree(w,cur,1,sigma);
	free(w);
	return true ;
}

void traverse( int x, int px, vector<pair<int,int> > &vec ) {
	int cx,cpx = which_chain[px];
	for ( ;(cx = which_chain[x]) != cpx; x = anc[chain[_left[cx]]][0] ) 
		vec.push_back(make_pair(_left[cx],_left[cx]+pos[x]));
	vec.push_back(make_pair(_left[cpx]+pos[px],_left[cx]+pos[x]));
}

vector<pair<int,int> > 
get_query_intervals( int x, int y, int *len ) {
	assert( x != y );
	int pre_lca_x, pre_lca_y, 
		z = lca(x,y,&pre_lca_x,&pre_lca_y);
	vector<pair<int,int> > vec;
	vec.clear();
	if ( z == x ) 
		*len = d[y]-d[z]+1, traverse(y,z,vec);
	else if ( z == y ) 
		*len = d[x]-d[z]+1, traverse(x,z,vec);
	else {
		assert( x != z && y != z );
		traverse(x,pre_lca_x,vec);
		traverse(y,pre_lca_y,vec);
		traverse(z,z,vec);
		*len = d[x]-d[pre_lca_x]+1 + d[y]-d[pre_lca_y]+1 + 1;
	}
	return vec;
}

int median_query( int x, int y ) {
	int len;
	if ( x == y ) return weight[x];
	vector<pair<int,int> > vec = get_query_intervals(x,y,&len);
	return wt->kth_weight(vec,len>>1);
}

int main( int argc, char **argv ) {
	int i,j,oqr,qr,cs = 0;
	double ax = 0;
	FILE *fp = fopen(argv[1],"w");
	for (;read_preprocess() && ++cs;) {
		auto start = std::chrono::high_resolution_clock::now();
		for ( scanf("%d",&qr), oqr = qr; qr-- && 2 == scanf("%d %d",&i,&j); ) {
			printf("%d\n",median_query(i,j)) ;
		}
		auto finish = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = finish - start;
		ax += elapsed.count()/oqr;
		fprintf(fp,"%.6lf\n",elapsed.count()/oqr);
	}
	fprintf(fp,"%.6lf\n",ax/cs);
	//printf("%lf\n",ax);
	fclose(fp);
	return 0;
}


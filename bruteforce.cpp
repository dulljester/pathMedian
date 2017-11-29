/*
 */
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <chrono>
using namespace std;
#define MAXLOG (21)
#define N (1<<MAXLOG)
#define MAXE (N<<1)
#define NONE (-1)

int d[N],n,E,last[N],_next[MAXE],to[MAXE],weight[N],p[N],seen[N],yes,anc[N][MAXLOG+1],K;

void add_arcs( int x, int y ) {
	int i = E++, j = E++;
	to[i] = y, _next[i] = last[x], last[x] = i;
	to[j] = x, _next[j] = last[y], last[y] = j;
}

void dfs( int x, int depth ) {
	int k,i,y;
	if ( seen[x] == yes )
		return ; 
	for ( d[x] = depth, seen[x] = yes, i = last[x]; i != NONE; i = _next[i] )
		if ( seen[y = to[i]] != yes ) {
			for ( p[y] = i, anc[y][0] = x, k = 1; anc[y][k-1] != NONE; anc[y][k] = anc[anc[y][k-1]][k-1], ++k ) ;
			dfs(y,1+depth);
		}
}

int up( int x, unsigned int u ) {
	for ( int k = 0; u; u >>= 1, ++k )
		if ( u & 1 ) x = anc[x][k];
	return x;
}

int lca( int x, int y ) {
	int k;
	if ( d[x] > d[y] )
		return lca(up(x,d[x]-d[y]),y);
	if ( d[x] < d[y] )
		return lca(y,x);
	if ( x == y )
		return x;
	for ( k = K-1; k; --k ) {
		assert( anc[x][k] == anc[y][k] );
		if ( anc[x][k-1] != anc[y][k-1] )
			x = anc[x][k-1], y = anc[y][k-1];
	}
	return anc[x][0];
}

bool read_preprocess() {
	int i,j,k,sigma;
	if ( 2 != scanf("%d %d",&n,&sigma) )
		return false ;
	for ( K = 0; (1<<K) <= n; ++K ) ;
	for ( i = 0; i < n; ++i )
		for ( k = 0; k < K; anc[i][k++] = NONE ) ;
	for ( E = 0, i = 0; i < n; last[i++] = NONE ) ;
	for ( i = 0; i < n; scanf("%d",&weight[i++]) ) ;
	for ( k = 0; k < n-1; ++k )
		scanf("%d %d",&i,&j), add_arcs(i,j);
	++yes, dfs(0,0);
	return true ;
}

int query( int x, int y ) {
	vector<int> w;
	if ( x == y ) return weight[x];
	int i,j,z = lca(x,y);
	for ( w.clear(); x != z; w.push_back(weight[x]), x = anc[x][0] ) ;
	for ( 		   ; y != z; w.push_back(weight[y]), y = anc[y][0] ) ;
	w.push_back(weight[z]);
	sort(w.begin(),w.end());
	return w[w.size()>>1];
}

int main() {
	int i,j,k,qr;
	double ax = 0;
	for ( ;read_preprocess(); ) {
		auto start = std::chrono::high_resolution_clock::now();
		for ( scanf("%d",&qr); qr-- && 2 == scanf("%d %d",&i,&j); printf("%d %d -> %d\n",i,j,query(i,j)) ) ;
		auto finish = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = finish - start;
		ax += elapsed.count();
	}
	printf("%lf\n",ax);
	return 0;
}


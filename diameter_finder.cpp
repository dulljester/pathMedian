/*
 */
#include <cassert>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <set>
#include <map>
#define BIT(k) (1ULL<<(k))
#define MASK(k) (BIT(k)-1ULL)
#define L(k) ((k)&((~(k))+1ULL))
#define tol (1e-13)
using namespace std;
typedef unsigned long long u64;
typedef long long i64;
#define N (1<<22)
enum { UP, DOWN };
#define oo (1LL<<60)

int n,p[N],seen[N],yes,weight[N];
vector<pair<int,int> > adj[N];
i64 z[2][N],d[N],len[N];

class far_manager {
private:
	int x,
		left, right, up,
		fleft, fright, fup;
	i64 dleft, dright, dup;
public:
	void init( int x ) {
		dleft = -oo, dup = dright = 0;
		left = -1, up = right = x;
		fleft = -1, fup = fright = x;
		this->x = x;
	}
	int get_left() const { return left; }
	int get_dleft() const { return dleft; }
	int get_fleft() const { return fleft; }
	int get_right() const { return right; }
	int get_dright() const { return dright; }
	int get_fright() const { return fright; }
	int get_up() const { return up; }
	int get_dup() const { return dup; }
	int get_fup() const { return fup; }

	void update( int y, int f, i64 dist, bool from_up ) {
		if ( from_up ) {
			if ( dup < dist ) 
				dup = dist, fup = f, up = y;
			return ;
		}
		if ( y != right && y != left ) {
			if ( dist >= dright ) {
				dleft = dright, left = right, fleft = fright;
				dright = dist, right = y, fright = f;
			}
			else if ( dist > dleft ) 
				dleft = dist, left = y, fleft = f;
			return ;
		}
		if ( y == right ) {
			if ( dist > dright )
				dright = dist, right = y, fright = f;
			return ;
		}
		assert( y == left );
		if ( dist > dleft )
			dleft = dist, left = y, fleft = f;
	}
};

far_manager fm[N];

void dfs( int x ) {
	assert( seen[x] != yes );
	seen[x] = yes;
	for ( int y,i = 0; i < (int)adj[x].size(); ++i )
		if ( seen[y = adj[x][i].first] != yes ) 
			p[y] = x, d[y] = d[x]+(len[y] = adj[x][i].second), dfs(y);
}

/* 
 * z[DOWN][x] is the longest path
 * that starts from "x" and goes downward;
 * z[UP][x] is the longest path
 * that starts from "x" and goes first to its parent
 * and then somewhere from its parent,
 * but not back to "x";
 */
i64 calc_z( int t, int x ) {

	assert( 0 <= x && x < n );

	if ( z[t][x] < +oo ) return z[t][x];

	int i,y,px;

	if ( t == DOWN ) {
		for ( i = 0; i < (int)adj[x].size(); ++i )
			if ( p[y = adj[x][i].first] == x ) {
				calc_z(DOWN,y);
				fm[x].update(y,fm[y].get_fright(),len[y]+fm[y].get_dright(),false);
			}
		return z[t][x] = fm[x].get_dright();
	}

	assert( t == UP );

	//upward path from root (p[x] == -1) has length 0:
	if ( (px = p[x]) == -1 ) 
		return z[t][x] = 0;

	// otherwise calculate best upward path from p[x]
	z[t][x] = len[x]+calc_z(t,px);
	fm[x].update(px,fm[px].get_fup(),len[x]+fm[px].get_dup(),true);

	if ( fm[px].get_right() != x && fm[px].get_left() != x ) {
		if ( fm[px].get_left() != -1 )
			fm[x].update(px,fm[px].get_fleft(),len[x]+fm[px].get_dleft(),true), z[t][x] = max(z[t][x],len[x]+fm[px].get_dleft());
		if ( fm[px].get_right()	!= -1 )
			fm[x].update(px,fm[px].get_fright(),len[x]+fm[px].get_dright(),true), z[t][x] = max(z[t][x],len[x]+fm[px].get_dright());
	}
	else if ( fm[px].get_right() == x ) {
		if ( fm[px].get_left() != -1 )
			fm[x].update(px,fm[px].get_fleft(),len[x]+fm[px].get_dleft(),true), z[t][x] = max(z[t][x],len[x]+fm[px].get_dleft());
	}
	else {
		if ( fm[px].get_right() != -1 )
			fm[x].update(px,fm[px].get_fright(),len[x]+fm[px].get_dright(),true), z[t][x] = max(z[t][x],len[x]+fm[px].get_dright());
	}

	return z[t][x];

}

vector<pair<int,int> > vec,orig_e;

int main() {
	int i,j,k,l,ts = 0,cs = 0,t,sigma;
	FILE *queries = fopen("queries.txt","w");
	for ( ;2 == scanf("%d %d",&n,&sigma); ) {
		for ( vec.clear(), orig_e.clear(), i = 0; i < n; p[i] = -1, fm[i].init(i), adj[i++].clear() ) ;
		for ( t = UP; t <= DOWN; ++t )
			for ( i = 0; i < n; z[t][i++] = +oo ) ;
		for ( i = 0; i < n; scanf("%d",&weight[i++]) ) ;
		for ( k = 0; k < n-1; ++k ) {
			scanf("%d %d",&i,&j);
			assert( 0 <= i && i < n );
			assert( 0 <= j && j < n );
			adj[i].push_back(make_pair(j,1));
			adj[j].push_back(make_pair(i,1));
			orig_e.push_back(make_pair(i,j));
		}
		++yes, d[0] = 0, dfs(0);
		for ( i = 0; i < n; calc_z(DOWN,i++) ) ;
		for ( i = 0; i < n; ++i ) {
			max(calc_z(UP,i),calc_z(DOWN,i));
			if ( fm[i].get_fright() != -1 && fm[i].get_fright() != i )
				vec.push_back(make_pair(i,fm[i].get_fright()));

			if ( fm[i].get_fleft() != -1 && fm[i].get_fleft() != i )
				vec.push_back(make_pair(i,fm[i].get_fleft()));

			if ( fm[i].get_fup() != -1 && fm[i].get_fup() != i )
				vec.push_back(make_pair(i,fm[i].get_fup()));
		}
		for ( fprintf(queries,"%d %d\n",n,sigma), i = 0; i < n; ++i )
			fprintf(queries,"%d ",weight[i]);
		fprintf(queries,"\n");
		int uu,vv;
		assert( orig_e.size() == n-1 );
		for ( i = 0; i < (int)orig_e.size(); ++i ) 
			fprintf(queries,"%d %d\n",orig_e[i].first,orig_e[i].second);
		for ( fprintf(queries,"%lu\n",vec.size()), i = 0; i < (int)vec.size(); ++i ) {
			fprintf(queries,"%d %d\n",uu = vec[i].first,vv = vec[i].second);
			assert( uu != vv );
		}
		printf("Test case %d done\n",++ts);
	}
	fclose(queries);
	return 0;
}


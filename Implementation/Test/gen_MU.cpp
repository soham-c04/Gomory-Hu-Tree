#include<bits/stdc++.h>
#include<ext/random>
using namespace std;
#define f(n) for(int i=0;i<(n);i++)
#define ao(a,n) for(int j=0;j<(n);j++) cout<<a[j]<<" ";
#define pb push_back
#define F first
#define S second
typedef long long ll;
typedef vector<int> vi;
typedef pair<int,int> pi;

using RNG = __gnu_cxx::sfmt19937;
//decltype(RNG()()) x;
ll rand_num(ll a, ll b){
	random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<ll> distrib(a, b);
    ll result = distrib(gen);
    return result;
}

int main(int argc, char* argv[]){
	int n=rand_num(100, 500);
	int M=n*(n-1)/2;
	int m=rand_num(n-1,M);
	pair<int,int> all_edges[M];
	for(int i=1,t=0;i<n;i++){
		for(int j=i+1;j<=n;j++){
			all_edges[t++]={i,j};
		}
	}
	vector<tuple<int,int,ll>> edges;
	int cap[n+1][n+1];
	memset(cap, 0, sizeof(cap));
	int W = 50;

	for(int t=0;t<m;t++){
		int x = rand_num(t,m-1);
		auto [u,v]=all_edges[x];
		swap(all_edges[x], all_edges[t]);
		int w = rand_num(1,W);
		edges.push_back({u,v,w});
		cap[u][v] = cap[v][u] = w;
	}

	cout<<n<<" "<<edges.size()<<"\n";
	for(auto [u,v,w]:edges) cout<<u<<" "<<v<<" "<<w<<"\n";

	// Multiple Dynamic Updates
	int q = rand_num(20,40);
	cout<<q<<"\n";
	while(q--){
		int u = rand_num(1,n-1);
		int v = rand_num(u+1,n);
		int delta = rand_num(-cap[u][v],W/2);
		cout<<u<<" "<<v<<" "<<delta<<"\n";
		cap[u][v] = cap[v][u] = cap[u][v]+delta;
	}

	return 0;
}

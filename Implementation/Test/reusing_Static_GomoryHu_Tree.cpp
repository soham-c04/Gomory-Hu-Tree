// Compiled with flags -std=c++20 -O3
#include "../Dinic.cpp"
// const ll INF = 1e18; (declared in Dinic.cpp)
// vector<vector<pair<int,ll>>> graph; (declared in Dinic.cpp)
#define F first
#define S second

int n;  						// Number of nodes in the graph
vector<vector<ll>> F;           // Final Flow Matrix

void inputGraph(){
	int m;
	cin>>n>>m;
	graph.resize(n);
	while(m--){
		int u,v,w;
		cin>>u>>v>>w;
		u--; v--;
		graph[u].push_back({v,w});
		graph[v].push_back({u,w});
	}
}


void create_GHT(){
	F.assign(n, vector<ll>(n, INF));

	int representative[n] = {0};
	ll fl[n];
	F[0][0] = 0;

	for(int s=1;s<n;s++){
		F[s][s] = 0;
		int t = representative[s];
		auto [S,cut] = find_minCut(s, t);
		fl[s] = cut;
		bool rt_in_S = false;
		for(int i:S){
			if(i!=s && representative[i]==t) representative[i] = s;
			if(i==representative[t]) rt_in_S = true;
		}
		if(rt_in_S){
			representative[s] = representative[t];
			representative[t] = s;
			fl[s] = fl[t];
			fl[t] = cut;
		}

		F[s][t] = F[t][s] = cut;
		for(int i=0;i<s;i++)
			if(i!=t) F[s][i] = F[i][s] = min(F[t][i], cut);
	}
}

auto Find(int v, int key){
	return find_if(graph[v].begin(), graph[v].end(), [key](const auto &p){ return p.F == key; });
}

void Increase(int b, int d, int delta){
	if(Find(b,d) != graph[b].end()){
		Find(b,d)->second += delta;
		Find(d,b)->second += delta;
	}
	else{
		graph[b].push_back({d,delta});
		graph[d].push_back({b,delta});
	}
}

void Decrease(int b, int d, int delta){
	Find(b,d)->second -= delta;
	Find(d,b)->second -= delta;
}

int main(int argc, char *argv[]){
	ios::sync_with_stdio(false);
	cin.tie(0);

	/*
	Compile-time argument:
	1. No argument run for benchmarks.
	2. Anything else to debug() [for Stress testing]
	*/
	bool DEBUG = (argc>1);

	inputGraph();
	create_GHT();

    auto start_time = chrono::high_resolution_clock::now();

	int q;
	cin>>q;
	while(q--){
		int b,d,delta;
		cin>>b>>d>>delta;
		b--; d--;

		if(delta>0) Increase(b,d,delta);
		else if(delta<0) Decrease(b,d,-delta);

		create_GHT();
		
		if(DEBUG){
			cout<<"\n";
			for(int i=0;i<n;i++){
				for(int j=0;j<n;j++){
					cout<<"f("<<i+1<<","<<j+1<<") = "<<F[i][j]<<"	";
				}
				cout<<"\n";
			}
			cout<<"\n";
		}
	}

	if(!DEBUG){
		auto end_time = chrono::high_resolution_clock::now();
		cout<<chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
	}

	return 0;
}

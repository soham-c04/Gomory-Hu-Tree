// Compiled with flags -std=c++20 -O3
#include "../Dinic.cpp"
// const ll INF = 1e18; (declared in Dinic.cpp)
// vector<vector<pair<int,ll>>> graph; (declared in Dinic.cpp)
#define F first
#define S second

int n;

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
	Works for both Static and Dynamic Gomory-Hu Tree.
	Use "1 0 0 0" as input to the query section for comparing with Static Gomory-Hu Tree

	Compile-time argument:
	1. No argument run for benchmarks.
	2. Anything else to debug() [for Stress testing]
	*/
	bool DEBUG = (argc>1);

	inputGraph();

    auto start_time = chrono::high_resolution_clock::now();
    
	int q;
	cin>>q;
	while(q--){
		int b,d,delta;
		cin>>b>>d>>delta;
		b--; d--;

		if(delta>0) Increase(b,d,delta);
		else if(delta<0) Decrease(b,d,-delta);

		if(DEBUG) cout<<"\n";
		int F[n][n];
		memset(F, -1, sizeof(F));
		for(int i=0;i<n;i++){
			F[i][i]=0;
			for(int j=0;j<n;j++){
				if(F[i][j]==-1){
					auto [S,cut] = find_minCut(i,j);
					F[i][j]=F[j][i]=cut;
				}
				if(DEBUG) cout<<"f("<<i+1<<","<<j+1<<") = "<<F[i][j]<<"	";
			}
			if(DEBUG) cout<<"\n";
		}
		if(DEBUG) cout<<"\n";
		
		if(q && (b==-1)) start_time = chrono::high_resolution_clock::now();
	}

	if(!DEBUG){
		auto end_time = chrono::high_resolution_clock::now();
		cout<<chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
	}

	return 0;
}

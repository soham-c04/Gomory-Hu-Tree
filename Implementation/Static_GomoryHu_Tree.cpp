// Compiled with flags -std=c++20 -O3
#include "Dinic.cpp"
// const ll INF = 1e18; (declared in Dinic.cpp)
// vector<vector<pair<int,ll>>> graph; (declared in Dinic.cpp)

int n;  						// Number of nodes in the graph
vector<vector<pair<int,ll>>> T; // Cut-Tree
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
	T.resize(n);
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


	for(int i=1;i<n;i++){
		T[i].push_back({representative[i],fl[i]});
		T[representative[i]].push_back({i,fl[i]});
	}
}

vector<vector<ll>> compute_flow_matrix(){
	vector<vector<ll>> F1(n, vector<ll>(n,0));
	int root;
	function<void(int,ll,int)> dfs = [&](int u, ll mn, int par){
        for(auto [v,w]:T[u]) {
            if(v!=par){
            	F1[root][v] = min(mn,w);
            	dfs(v,F1[root][v],u);
			}
        }
    };

	for(root=0;root<n;root++) dfs(root,INF,-1);
	return F1;
}

void debug(){
	auto F1 = compute_flow_matrix();
	cout<<"\n";
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++){
			ll cut = 0;
			if(i!=j){
				auto [S,Cut] = find_minCut(i,j);
				cut = Cut;
				swap(T,graph);
				auto [Ts,Tcut] = find_minCut(i,j);
				swap(T,graph);
				for(int s:Ts){
					for(auto [v,w]:graph[s]){
						if(find(Ts.begin(),Ts.end(),v) == Ts.end()) Tcut -= w;
					}
				}
				if(Tcut){
					cout<<"*";
//						for(int s:Ts) cout<<s+1<<" ";
					cout<<"*";
				}
			}
			if((F[i][j]!=cut) || (F[i][j]!=F1[i][j])) cout<<"*";

			cout<<"f("<<i+1<<","<<j+1<<") = "<<F[i][j]<<"	";
		}
		cout<<"\n";
	}
	cout<<"\n";
}

void printTree(){
	cout<<"\nCut-Tree:-\n";
	for(int i=0;i<n;i++){
		for(auto [v,w]:T[i]) if(v>i) cout<<i+1<<" "<<v+1<<" "<<w<<"\n";
	}
}

int main(int argc, char *argv[]){
	ios::sync_with_stdio(false);
	cin.tie(0);
	
	/*
	Compile-time argument:
	1. No argument runs normally and prints the Cut-Tree.
	2. "1" to debug() [for Stress testing]
	3. Any other argument is used for benchmarking purposes.
	*/

	inputGraph();

    auto start_time = chrono::high_resolution_clock::now();
	create_GHT();
	
	if(argc>1){     
		if(strcmp(argv[1],"1")==0) debug();
		else{
		    auto end_time = chrono::high_resolution_clock::now();
		    cout<<chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
		}
		return 0;
	}

	printTree();

	return 0;
}

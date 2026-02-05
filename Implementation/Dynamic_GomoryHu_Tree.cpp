// Compiled with flags -std=c++20 -O3
#include "Dinic.cpp"
// const ll INF = 1e18; (declared in Dinic.cpp)
// vector<vector<pair<int,ll>>> graph; (declared in Dinic.cpp)

#define F first
#define S second

int n;							// Number of nodes in the graph
vector<vector<ll>> adj;         // Adjacency Matrix
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

	adj.assign(n, vector<ll>(n,0));
	for(int i=0;i<n;i++) for(auto [v,w]:graph[i]) adj[i][v] = w;
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

vector<bool> get_path(int u,int v){	// O(n)
	vector<bool> in_path(n,false);
	int par[n];
	memset(par, - 1, sizeof(par));
	queue<int> bfs;
	bfs.push(u);
	while(bfs.size()){
		int p = bfs.front();
		bfs.pop();
		if(p == v){
			while(p!=-1){
				in_path[p] = true;
				p = par[p];
			}
			break;
		}
		for(auto [q,w]:T[p]){
			if(q != par[p]){
				par[q] = p;
				bfs.push(q);
			}
		}
	}
	return in_path;
}

bool is_bridge(int u,int v){		// Corollary 2: O(1)
	return F[u][v] == adj[u][v];
}

auto Find(int v, int key){
	return find_if(T[v].begin(), T[v].end(), [key](const auto &p){ return p.F == key; });
}

void Increase(int b, int d, int delta){
	if(adj[b][d]){
		find(graph[b].begin(), graph[b].end(), make_pair(d,adj[b][d]))->second += delta;
		find(graph[d].begin(), graph[d].end(), make_pair(b,adj[d][b]))->second += delta;
	}
	else{
		graph[b].push_back({d,delta});
		graph[d].push_back({b,delta});
	}
	vector<bool> in_path = get_path(b,d);
	if(F[b][d] == 0){   // Lemma 4
		for(int i=0;i<n;i++){
			if(in_path[i]){
				int j=-1;
				for(auto [v,w]:T[i]){
					if(w==0 && in_path[v]){
						j = v;
						break;
					}
				}
				if(j!=-1){
					// Remove edge on path with weight = 0
					T[i].erase(Find(i,j));
					T[j].erase(Find(j,i));
					// Insert new edge with delta weight
					T[b].push_back({d,delta});
					T[d].push_back({b,delta});
					break;
				}
			}
		}
	}
	// else if(F[b][d] == adj[b][d]) i.e. Corollary 1
	else{
		int parent[n];
		memset(parent,-1,sizeof(parent));
		int fl[n];
		for(int s=0;s<n;s++){
			if(in_path[s]){
				parent[s] = b;
				for(auto [v,w]:T[s]){
					if(!in_path[v]){
						parent[v] = b;
						fl[v] = w;
						T[v].erase(Find(v,s));
					}
				}
				T[s].clear();
			}
		}

		in_path[b] = false;
		for(int s=0;s<n;s++){
			if(!in_path[s]) continue;
			int t = parent[s];
			auto [S,cut] = find_minCut(s, t);   // O(n*n*m)
			fl[s] = cut;
			bool rt_in_S = false;
			for(int i:S){
				if(i!=s && parent[i]==t) parent[i] = s;
				if(i==parent[t]) rt_in_S = true;
			}
			if(rt_in_S){
				parent[s] = parent[t];
				parent[t] = s;
				fl[s] = fl[t];
				fl[t] = cut;
			}
		}

		parent[b] = -1;
		for(int i=0;i<n;i++){
			if(parent[i]!=-1){
				T[i].push_back({parent[i],fl[i]});
				T[parent[i]].push_back({i,fl[i]});
			}
		}
	}

	adj[b][d] = adj[d][b] = adj[b][d]+delta;
}

void Decrease(int b, int d, int delta){
	// Update graph
	auto it_b = find(graph[b].begin(), graph[b].end(), make_pair(d,adj[b][d]));
	auto it_d = find(graph[d].begin(), graph[d].end(), make_pair(b,adj[d][b]));
	if(delta == adj[b][d]){
		graph[b].erase(it_b);
		graph[d].erase(it_d);
	}
	else{
		it_b->second -= delta;
		it_d->second -= delta;
	}

	if(is_bridge(b,d)){ 					// Lemma 5: Decrease corresponding edge in tree by delta
		Find(b,d)->second -= delta;
		Find(d,b)->second -= delta;
		adj[b][d] = adj[d][b] = adj[b][d]-delta;
		return;
	}

	vector<bool> in_path = get_path(b,d);	// Gives if a certain vertex is present in path from b to d
	vector<vector<int>> N(n);   			// Neighbors of a vertex in the path
	set<tuple<ll,int,int>> Q;               // Thin edges on the path in increasing order of weights
	for(int i=0;i<n;i++){
		if(in_path[i]){
			for(int j=0;j<T[i].size();j++){
				auto [v,w] = T[i][j];
				if(!in_path[v]) Q.insert({w,i,v});
				else{
					T[i][j].S -= delta;  	// Lemma 1(ii)
					N[v].push_back(i);
				}
			}
		}
 	}
	while(!Q.empty()){
		auto it = --Q.end();
		auto [e,v,u] = *it;
		Q.erase(it);
		ll L = Find(v,N[v][0])->second;
		if(N[v].size() == 2) L = min(L, Find(v,N[v][1])->second);
		if((L>=e) || (e == adj[u][v])) continue;   // If (Lemma 7 or Corollary 3) then apply Lemma 6
		auto [U,cut] = find_minCut(u,v);
		if(e == cut) continue;						// Lemma 6
		Find(u,v)->second = cut;					// Update edge u-v in Tree
		Find(v,u)->second = cut;
		bool in_U[n] = {0};
		for(int i:U) in_U[i] = true;
		vector<int> disconnect;

		// O(n*n) - Can be possibly optimized using parent[] and fl[] array similar to Increase()
		for(auto [x,wx]:T[v]){
			if(in_U[x] && (x!=u)){
				if(in_path[x]){
					if(N[v][0] == x) N[v][0] = u;
					else N[v][1] = u;
					N[u] = {v,x};
					if(N[x][0] == v) N[x][0] = u;
					else N[x][1] = u;
				}
				disconnect.push_back(x);
				Q.erase({wx,v,x});
				// Disconnect x from v
				T[x].erase(Find(x,v));
				// Reconnect x to u
				T[u].push_back({x,wx});
				T[x].push_back({u,wx});
			}
		}
		for(int x:disconnect) T[v].erase(Find(v,x));
		for(auto [to,w]:T[u]) if(!in_path[to]) Q.insert({w,u,to});

		in_path[u] = true;
	}
	adj[b][d] = adj[d][b] = adj[b][d]-delta;
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
			if(F[i][j]!=cut) cout<<"*";

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
	
	int CASE=0;
	if(argc>1){
		if(strcmp(argv[1],"1")==0) CASE=2;
		else CASE=1;
	}

	inputGraph();
	create_GHT();

	auto start_time = chrono::high_resolution_clock::now();
	
	int q;
	cin>>q;
	while(q--){
		int b,d,delta;
		cin>>b>>d>>delta;
		b--; d--;
		if(b>d) swap(b,d);
		
		if(delta>0) Increase(b,d,delta);
		else if(delta<0) Decrease(b,d,-delta);
		F = compute_flow_matrix(); // Whole recomputation can be optimized ?

		if(CASE==0) printTree();
		else if(CASE==2) debug();
	}

	if(CASE==1){
		auto end_time = chrono::high_resolution_clock::now();
		cout<<chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
	}

	return 0;
}

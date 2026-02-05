#include "Dinic.cpp"

int createGraph(){
	int n=6;
	graph.resize(n);
	graph[0] = {{1,10}, {5,8}};
	graph[1] = {{0,10}, {5,3}, {4,2}, {2,4}};
	graph[2] = {{1,4}, {5,2}, {4,4}, {3,5}};
	graph[3] = {{2,5}, {5,2}, {4,7}};
	graph[4] = {{5,3}, {1,2}, {2,4}, {3,7}};
	graph[5] = {{0,8}, {1,3}, {2,2}, {3,2}, {4,3}};

	return n;
}

pair<vector<vector<pair<int,ll>>>,vector<vector<ll>>> EFT(int n){
	vector<vector<pair<int,ll>>> tree(n);
	vector<vector<ll>> flow_matrix(n, vector<ll>(n,INF));
	
	int parent[n] = {0}; parent[0] = -1;
	flow_matrix[0][0] = 0;
	
	for(int s=1;s<n;s++){
		flow_matrix[s][s] = 0;
		int t = parent[s];
		auto [S,cut] = find_minCut(s, t);
		tree[s].push_back({t,cut});
		tree[t].push_back({s,cut});
		for(int i:S)
			if(i>=s && parent[i]==t) parent[i] = s;
		flow_matrix[s][t] = flow_matrix[t][s] = cut;
		for(int i=0;i<s;i++)
			if(i!=t) flow_matrix[s][i] = flow_matrix[i][s] = min(flow_matrix[t][i], cut);
	}
	
	return {tree,flow_matrix};
}

int main(){
	int n = createGraph();
	
	auto [T_, F] = EFT(n);
	
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++)
			cout<<"f("<<i<<","<<j<<") = "<<F[i][j]<<"	";
		cout<<"\n";
	}
	
	return 0;
}

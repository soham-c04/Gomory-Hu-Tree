#include "../Dinic.cpp"

vector<vector<pair<int,ll>>> create_GHT(vector<vector<pair<int,ll>>> &input_graph){
	swap(graph, input_graph);
	int n = graph.size();
	int representative[n] = {0};
	ll fl[n];

	for(int s=1;s<n;s++){
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
	}
	swap(graph, input_graph);

	vector<vector<pair<int,ll>>> T(n);
	for(int i=1;i<n;i++){
		T[i].push_back({representative[i],fl[i]});
		T[representative[i]].push_back({i,fl[i]});
	}
	return T;
}

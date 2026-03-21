// Compiled with flags -std=c++20 -O3
#include "../Dinic.cpp"
#include "Image.cpp"
// vector<vector<pair<int,ll>>> graph; (declared in Dinic.cpp)

vector<vi> GraphClustering(int k, int m){
	int n = graph.size();

	int representative[n] = {0};
	ll fl[n];
	pair<ll,int> cuts[n-1];

	cout<<"\nProgress:"<<endl;
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
		
		// Print Progress
		{
			int percent = 100*s/(n-1);
			if(((percent%5)==0) && (percent>100*(s-1)/(n-1))) cout<<percent<<"% Completed\n";
		}
	}
	cout<<"\nGomory-Hu Tree constructed"<<endl;
	
	for(int i=1;i<n;i++) cuts[i-1]={fl[i],i};
	sort(cuts,cuts+n-1);
	
	vector<vi> clusters(k);
	clusters[0].push_back(0);
	representative[0] = 0;
	for(int i=1;i<k;i++){
	    int root = cuts[i-1].second;
	    clusters[i].push_back(root);
	    representative[root] = -i;
	}
	for(int i=1;i<n;i++){
		vi stk;
		int cur=i;
		while(representative[cur]>0){
			stk.push_back(cur);
			cur = representative[cur];
		}
		int root = -representative[cur];
		for(int j:stk){
			clusters[root].push_back(j);
			representative[j] = -root;
		}
	}
	
	cout<<"\nCluster sizes:";
	for(int i=0;i<k;i++){
		cout<<"Cluster "<<i+1<<": "<<clusters[i].size()<<endl;
		if(clusters[i].size()<10){
			cout<<"Pixels: ";
			for(int j:clusters[i]) printf("(%d,%d)  ",j/m,j%m);
			cout<<endl;
		}
	}
	return clusters;
}

int main(int argc, char *argv[]){
	/*
	Compile-time argument:
	1. No argument runs normally and prints the Cut-Tree.
	2. Any other argument is used for benchmarking purposes.
	*/

	string path;
	cout<<"Enter relative path to images/your_image(.png): ";
	cin>>path;
	path = "images/" + path;

	vector<vector<vi>> image = inputImage(path);
	cout<<"\nSuccessfully decoded image"<<endl;

	int k;
	cout<<"\nEnter number of clusters: ";
	cin>>k;

	cout<<"\nClustering..."<<endl;
    graph = ImageToGraph(image);
    
    auto start_time = chrono::high_resolution_clock::now();
	vector<vector<int>> clusters = GraphClustering(k, image[0].size());
	auto end_time = chrono::high_resolution_clock::now();
	
	cout<<"\nClustering Complete.\nConverting back to Image(.png)..."<<endl;
	GraphToImage(path, clusters, image);
	cout<<"\nDone"<<endl;
	
	long long Time = chrono::duration_cast<chrono::seconds>(end_time - start_time).count();
 	if(argc>1) cout<<Time;
	else cout<<"\nExecution time: "<<Time<<" seconds"<<endl;

	return 0;
}

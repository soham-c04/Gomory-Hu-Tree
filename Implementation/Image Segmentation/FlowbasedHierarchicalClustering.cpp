// Compiled with flags "-std=c++20 -O3 -Wall -Wextra -Wl,--stack,1073741824"
#include "GomoryHu_Tree.cpp"
#include "Image.cpp"
#define ceil(a,b) (((a)+(b)-1)/(b))
#include <stack>
const double FACTOR = 0.01;
const int THRESHOLD = cMAX*FACTOR;  // Separate into clusters if maxflow(a,b) < THRESHOLD
const int WINDOW = 2;           	// pixels*pixels per subgraph after initial clustering
const int MIN_SIZE = 100;       	// Minimum number of pixels in a cluster
long long flowTime = 0;

const bool DEBUG = false;
void debug(const string &dbg, const bool condition=true){
	if(DEBUG && condition) cout<<"\nDEBUG	[  "<<dbg<<"  ]"<<endl;
}

void error(const string &err, int exit_code, bool condition){
	if(condition){
		cout<<"\n\nERROR: "<<err<<endl;
		exit(exit_code);
	}
}

vector<vi> merge(vector<vi> clusters, vector<vector<pair<int,ll>>> &graph){
	int n = graph.size();
	bool small=true;
	while(small && (clusters.size()>1)){
		small = false;
		int k = clusters.size();
		for(int i=0;i<k;i++){
			if(clusters[i].size()>MIN_SIZE) continue;
			small = true;
			int partof[n];
			for(int j=0;j<k;j++) for(int c:clusters[j]) partof[c] = j;
			ll boundary[k]={0};
			ll max_boundary = -1;
			int merge_to=-1;
			for(int u:clusters[i]){
				for(auto [v,capacity]:graph[u]){
					int pv = partof[v];
					if(pv!=i){
						boundary[pv]+=capacity;
						if(boundary[pv]>max_boundary){
							max_boundary = boundary[pv];
							merge_to = pv;
						}
					}
				}
			}
			for(int u:clusters[i]) clusters[merge_to].pb(u);
			clusters.erase(clusters.begin()+i);
			break;
		}
	}
	return clusters;
}

vector<vi> partition(vector<vector<pair<int,ll>>> &T){  // Clustering only based on Threshold
	const int n = T.size();
	vector<vi> parts;
	bool vis[n]={0};
	for(int i=0;i<n;i++){
		if(vis[i]) continue;
		parts.pb({});
		stack<int> stk;
		stk.push(i);
		vis[i] = true;
		while(stk.size()){
			int u = stk.top();
			stk.pop();
			parts.back().pb(u);
			for(auto [v,capacity]:T[u]){
				if((!vis[v]) && (capacity>=THRESHOLD)){
					vis[v] = true;
					stk.push(v);
				}
			}
		}
	}
	return parts;
}

vector<vi> GraphClustering(vector<vector<pair<int,ll>>> graph, const int cols){
	vector<vector<pair<int,ll>>> original_graph = graph;
	const int n = graph.size();
	const int rows=n/cols;
	int subgraphs = ceil(rows,WINDOW)*ceil(cols,WINDOW);
	vector<vi> nodes(subgraphs);    // Stores the set of nodes in each local subgraph G'_i,m
	vi partof(n,-1);       			// Stores "which" subgraph, each node is a part of.

	/* --- Step 1: Partition G into number of small subgraphs --- */
	// O(n) amortized [n = number of nodes in the graph]
	for(int i=0;i<rows;i++){
		for(int j=0;j<cols;j++){
			int m = (i/WINDOW)*ceil(cols,WINDOW) + (j/WINDOW);
			int v = i*cols+j;
			nodes[m].push_back(v);
			partof[v] = m;
		}
	}

	vi original(n);          	// Mapping condensed (root) node to original nodes
	vector<pair<int,ll>> t0[n];	// Condensed part of orignial tree
	for(int i=0;i<n;i++) original[i]=i;

	int total_node_condensations=0, total_edge_condensations=0;
	const int BRUTE = 1;
	for(int i=0;i<10 && subgraphs>1;i++){
		int n1 = n-total_node_condensations;// Number of nodes in G_i
		if(n1<BRUTE) break;
		cout<<"\nLevel "<<i<<":  "<<subgraphs<<" subgraphs  -  ";
		int node_condensations = 0, edge_condensations = 0;
		int compress[n1];  					// Compressing node indices to a smaller range inside the local subgraph. Used to create G_i_m
		int compress_c[n1];                 // Compressing node indices again after node condensation. Used to create G_c
		memset(compress, -1, sizeof(compress));
		memset(compress_c, -1, sizeof(compress_c));
		vi original_c, partof_c;
		vector<map<int,ll>> G_c;          	// Condensed/reindexed graph G_i -> G_i_c
		vector<map<int,int>> sz;            // Number of condensed vertices under each node. Used for normalizing capacity.

		const int di=1, dm=84;
		for(int m=0;m<subgraphs;m++){
			int ni = nodes[m].size();   	// No. of nodes in local subgraph
			error(format("Empty Local Subgraph: Level = {}  m = {}",i,m),3,ni==0);
			int count=0;                	// Generating new indices for index compression. Used to create G_i_m.
			int decompress[ni];     		// Revert to original node indices before index compression.
			int root[ni];         			// "root" node in which the vertex is condensed to
			vi expand[ni];                  // Expands "root" node into constituent vertices. [Inverts root[] ]
			memset(decompress, -1, sizeof(decompress));
			memset(root, -1, sizeof(root));
			// Create (Separate out) Local Subgraph [G'_i,m]
			// Amortized O(n1+e1) over all m subgraphs [ e1 = No. of edges in G_i ]
			bool internal[ni]={0};     		// Node is internal or not ?
			vector<vector<pair<int,ll>>> G_i_m(ni);
			for(int u:nodes[m]){
				error(format("Vertex {} points to subgraph {} instead of {}",u,partof[u],m),1,partof[u]!=m);
				int U = compress[u];
				if(U==-1) compress[u]=U=count++;
				decompress[U] = u;
				bool internal_ = true;
				for(auto [v,capacity]:graph[u]){
					if(partof[u]==partof[v]){		// Both are part of same subgraph
						int V = compress[v];
						if(V==-1) compress[v]=V=count++;
						G_i_m[U].pb({V,capacity});
					}
					else internal_ = false;
				}
				internal[U] = internal_;
			}

			debug("Local subgraph created " + to_string(m),i==di && m==dm);
			if(DEBUG && i==di && m==dm){
				cout<<"ni = "<<ni<<endl;
				for(int U=0;U<ni;U++){
					cout<<U<<": ";
					for(auto [V,w]:G_i_m[U]) printf("(%d,%lld)  ",V,w);
					cout<<endl;
				}
			}
			/* --- Step 2: Find equivalent tree of T'*_i,m of G'_i,m using GomoryHu --- */
			// O(ni * maxflow(G_i_m)) = O(ni*ni*ei) [ei = Total edges inside G_i_m ]
			auto start_time = chrono::high_resolution_clock::now();
			vector<vector<pair<int,ll>>> T_i_m = create_GHT(G_i_m);
			auto end_time = chrono::high_resolution_clock::now();
			flowTime += chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
			debug("Step 2 completed",i==di && m==dm);
			
			// decompress[] => maps local subgraph indices to vertices at global graph at level i: G_i_m -> G_i
			// root[] => maps local subgraph vertices at level i to reindexed/condensed nodes in graph at level i: G_i_m -> G_i_m_c
			// expand[] => maps "root" vertex to set of condensed nodes in graph at level i: G_i_m_c -> G_i_m
			// original[] => maps reindexed/condensed nodes in graph at level i to original vertices: G_i -> G

			/* --- Step 3(i): Permanently condense all vertices inside T_i_m having edge capacities >= THRESHOLD --- */
			// O(ni)
			bool vis[ni]={0};
			int root0=-1;   						// Vertex for starting dfs() to find "maximal_internal_branch"
			for(int S=0;S<ni;S++){
				if(vis[S]) continue;
				vi components;		                // Components under the same condensed root
				stack<int> stk;
				stk.push(S);
				vis[S] = true;
				int s = decompress[S];
				int s0 = original[s];
				bool internal_ = true;
				while(stk.size()){
					int U = stk.top();
					stk.pop();
					components.pb(U);
					int u0 = original[decompress[U]];
					if(u0!=s0){
						t0[s0].pb({u0, THRESHOLD});
						t0[u0].pb({s0, THRESHOLD});
					}
					root[U] = S;
					internal_ = internal_&&internal[U];
					for(auto [V,capacity]:T_i_m[U]){
						if((!vis[V]) && (capacity>=THRESHOLD)){        // Condense v into u
							vis[V] = true;
							stk.push(V);
						}
					}
				}
				for(int C:components) internal[C] = internal_;
				if(!internal[S]) root0=S;
			}
			debug("Step 3(i) completed",i==di && m==dm);

			/* --- Step 3(ii): Temporarily condense all maximal interior branches of T_i_m --- */
			// O(ni)
			vi maximal_internal_branch;
			function<bool(int, int)> dfs = [&](int U, int par){
				bool internal_ = internal[U];
				vector<vi> branches;
				for(auto [V,capacity]:T_i_m[U]){
					if(V!=par){
						internal_=internal_&&dfs(V,U);
						if(maximal_internal_branch.size()){
							branches.pb({});
							swap(branches.back(), maximal_internal_branch);
						}
					}
				}
				if(internal_){
					maximal_internal_branch.pb(U);
					for(vi &branch:branches) for(int B:branch) maximal_internal_branch.pb(B);
				}
				else{
					for(vi &branch:branches){
						int R = branch[0];
						for(int B:branch) root[B] = R;
						for(int B:branch){
							int b0 = original[decompress[B]];
							for(auto [C,capacity]:T_i_m[B]){
								if(root[C]==R) t0[b0].pb({original[decompress[C]], capacity});
							}
						}
					}
				}
				return internal_;
			};
			dfs(root0,-1);
			
			for(int U=0;U<ni;U++) expand[root[U]].pb(U);
			debug("Step 3(ii) completed",i==di && m==dm);

			// Building new condensed graph G_c
			// Amortized O(n1) over all m subgraphs
			vi nodes_c;
			for(int U=0;U<ni;U++){
				if(U!=root[U]) continue;
				node_condensations += expand[U].size()-1;
				int u = decompress[U], u_c=G_c.size();
				compress_c[u] = u_c;
				original_c.pb(original[u]);
				partof_c.pb(partof[u]);
				nodes_c.pb(u_c);
				G_c.pb({});
				sz.pb({});
				for(int C:expand[U]) compress_c[decompress[C]] = u_c;
				for(int C:expand[U]){
					int u = decompress[C];
					for(auto [v,capacity]:graph[u]){
						int v_c = compress_c[v];
						if(v_c!=-1){
							if(v_c!=u_c){
								G_c[u_c][v_c]+=capacity;
								++sz[u_c][v_c];
							}
							else edge_condensations++;
						}
					}
				}
			}
			swap(nodes[m], nodes_c);
		}
		edge_condensations/=2;
		debug("Subgraph processing completed",i==di);
		
		// Update graph -> G_i_c
		// O(n1 + e1 - node_condensations - edge_condensations) = O(n2 + e2)
		int n2 = G_c.size();
		error(format("Number of nodes {} instead of {}",n2,n-total_node_condensations-node_condensations), 2, n2!=n-total_node_condensations-node_condensations);
		graph.clear();
		graph.resize(n2);
		for(int u_c=0;u_c<n2;u_c++){
			for(auto [v_c,capacity]:G_c[u_c]){
				int comp = sz[u_c][v_c];
				graph[u_c].pb({v_c, capacity/comp});
				graph[v_c].pb({u_c, capacity/comp});
			}
		}
		
		/* --- Step 5: Group subgraphs by joining inter-subgraph arcs: G_i_c -> G_(i+1) --- */
		// O(n2 + e2)
		subgraphs=0;
		vector<vi> nodes2;
		partof = vi(n2, -1);
		for(int m=0;m<(int)nodes.size();m++){
			if(nodes[m].empty()) continue;
			int v1=-1;
			int sg = subgraphs;
			for(int u:nodes[m]){
				for(auto [v,capacity]:graph[u]){    // Merge any 2 subgraphs sharing a commong edge
					int sv=partof_c[v];
					if(sv!=m){
						v1 = v;
						if(nodes[sv].size()){
							nodes2.pb({});
							swap(nodes2[sg],nodes[sv]);
							subgraphs++;
							break;
						}
					}
				}
				if(sg!=subgraphs) break;
			}
			error(format("Unmerged subgraph: Level = {}  m = {}",i,m), 4, v1==-1);
			if(sg==subgraphs) sg=partof[v1];
			for(int a:nodes[m]) nodes2[sg].pb(a);
			nodes[m].clear();
			for(int a:nodes2[sg]) partof[a] = sg;
		}
		swap(nodes, nodes2);
		swap(original, original_c);

		// Percentage condensation signifies - percentage of nodes/edges "in G_i" that were condensed. NOT in original graph.
		printf("%d (%.2f %%) nodes condensed  -  %d (%.2f %%) edges condensed\n",node_condensations,node_condensations*100.00/n1,edge_condensations,edge_condensations*100.00/(2*rows*cols-rows-cols-total_edge_condensations));
		fflush(stdout);
		total_node_condensations += node_condensations;
		total_edge_condensations += edge_condensations;
	}

	printf("\nTotal:  %d (%.2f %%) nodes condensed  -  %d (%.2f %%) edges condensed\n",total_node_condensations,total_node_condensations*100.00/n,total_edge_condensations,total_edge_condensations*100.00/(2*rows*cols-rows-cols));
	cout<<"\nRemaining: Nodes = "<<n-total_node_condensations<<"	Edges = "<<2*rows*cols-rows-cols-total_edge_condensations<<endl;

	/* --- Step 4: Create T'*_c if only one subgraph remains --- */
	auto start_time = chrono::high_resolution_clock::now();
	vector<vector<pair<int,ll>>> T1_c = create_GHT(graph);
	auto end_time = chrono::high_resolution_clock::now();
	flowTime += chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
	
	/* --- Step 6: Expand back temporarily condensed vertices in T'*_c to form T_c --- */
	vector<vector<pair<int,ll>>> T_c(n);
	for(int i=0;i<(int)T1_c.size();i++) for(auto [j,capacity]:T1_c[i]) T_c[original[i]].pb({original[j],capacity});
	for(int i=0;i<n;i++) for(auto edge:t0[i]) T_c[i].pb(edge);
	

	/* --- Step 7: Form clusters along with some Clustering Condition --- */
	vector<vi> clusters = partition(T_c);
	int k = clusters.size();
	cout<<"\n(Before merging) Number of clusters: "<<k<<endl;
	clusters = merge(clusters, original_graph);
	k = clusters.size();
	cout<<"\n(After merging) Number of clusters: "<<k<<endl;
	cout<<"\nCluster sizes:\n";
	for(int i=0;i<k;i++) cout<<"	Cluster "<<i+1<<": "<<clusters[i].size()<<endl;
	return clusters;
}

int main(int argc, char *argv[]){
	/*
	Compile-time argument:
	1. No argument runs normally and prints the cluster details.
	2. Any other argument is used for benchmarking purposes.
	*/

	string path;
	cout<<"Enter relative path to images/your_image(.png): ";
	cin>>path;
	path = "images/" + path;

	vector<vector<vi>> image = inputImage(path);
	cout<<"\nSuccessfully decoded image"<<endl;

	cout<<"\nClustering..."<<endl;
    vector<vector<pair<int,ll>>> graph = ImageToGraph(image);

    auto start_time = chrono::high_resolution_clock::now();
	vector<vector<int>> clusters = GraphClustering(graph, image[0].size());
	auto end_time = chrono::high_resolution_clock::now();

	cout<<"\nClustering Complete.\nConverting back to Image(.png)..."<<endl;
	GraphToImage(path, clusters, image);
	cout<<"\nDone"<<endl;

	long long Time = chrono::duration_cast<chrono::seconds>(end_time - start_time).count();
 	if(argc>1) cout<<Time;
	else cout<<"\nExecution time: "<<Time<<" seconds   -   maxFlow Computation Time: "<<flowTime/1e3<<" seconds"<<endl;

	return 0;
}

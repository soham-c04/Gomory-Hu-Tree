#include <vector>
#include <queue>
using namespace std;

typedef vector<int> vi;
typedef long long ll;

typedef class PushRelabelImp{
	private:
		static const int inf = 1000000000;
	public:
		int n;
		vector<vi> capacity, flow;
	private:
		vi height, excess;

		void push(int u, int v){
		    int d = min(excess[u], capacity[u][v] - flow[u][v]);
		    flow[u][v] += d;
		    flow[v][u] -= d;
		    excess[u] -= d;
		    excess[v] += d;
		}

		void relabel(int u){
		    int d = inf;
		    for(int i=0;i<n;i++){
		        if(capacity[u][i]-flow[u][i]>0) d = min(d, height[i]);
		    }
		    if(d<inf) height[u]=d+1;
		}

		vi find_max_height_vertices(int s, int t){
		    vi max_height;
		    for (int i=0;i<n;i++){
		        if(i!=s && i!=t && excess[i]>0){
		            if(!max_height.empty() && height[i] > height[max_height[0]]) max_height.clear();
		            if(max_height.empty() || height[i] == height[max_height[0]]) max_height.push_back(i);
		        }
		    }
		    return max_height;
		}

	public:
		PushRelabelImp(vector<vector<pair<int,ll>>> &graph){
   			n = graph.size();
		    capacity.assign(n, vi(n, 0));
   			for(int u=0;u<n;u++){
				for(auto [v,w]:graph[u]){
					capacity[u][v] += w;
				}
			}
		}
		
		int max_flow(int s, int t){
		    height.assign(n, 0);
		    height[s] = n;
		    flow.assign(n, vi(n, 0));
		    excess.assign(n, 0);
		    excess[s] = inf;
		    for(int i=0;i<n;i++){
		        if(i!=s) push(s,i);
		    }

		    vi current;
		    while(!(current=find_max_height_vertices(s, t)).empty()){
		        for(int i:current){
		            bool pushed = false;
		            for(int j=0;j<n && excess[i];j++){
		                if(capacity[i][j]-flow[i][j]>0 && height[i]==height[j]+1){
		                    push(i, j);
		                    pushed = true;
		                }
		            }
		            if(!pushed){
		                relabel(i);
		                break;
		            }
		        }
		    }

		    return excess[t];
		}

} PRI;

pair<vi, int> find_minCut(vector<vector<pair<int,ll>>> &graph, int s, int t){
	PRI mf(graph);
	int n = mf.n;
    int maxFlow = mf.max_flow(s,t);

	bool vis[n] = {0};
    queue<int> q;

    // BFS on residual graph
    q.push(s);
    vis[s] = true;
    while(!q.empty()){
        int u=q.front();
		q.pop();
        for(int v=0;v<n;v++){
            // residual capacity exists
            if(!vis[v] && mf.capacity[u][v] - mf.flow[u][v] > 0){
                vis[v] = true;
                q.push(v);
            }
        }
    }

    // collect vertices on S side
    vi S;
    for(int i=0;i<n;i++){
        if(vis[i]) S.push_back(i);
    }

    return {S, maxFlow};
}

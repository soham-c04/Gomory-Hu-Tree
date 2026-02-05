#include <bits/stdc++.h>
using namespace std;

typedef vector<int> vi;
typedef long long ll;
const ll INF = 1e18;

// 0-based indexing of nodes assumed throughout

vector<vector<pair<int,ll>>> graph;

struct FlowEdge {
    int v, u;
    ll cap, flow = 0;
    FlowEdge(int v, int u, ll cap) : v(v), u(u), cap(cap) {}
};

struct Dinic {
    const ll flow_inf = 1e18;
    vector<FlowEdge> edges;
    vector<vector<int>> adj;
    int n, m = 0;
    int s, t;
    vector<int> level, ptr;
    queue<int> q;

    Dinic(int n, int s, int t) : n(n), s(s), t(t) {
        adj.resize(n);
        level.resize(n);
        ptr.resize(n);
    }

    void add_edge(int v, int u, ll cap) {
        edges.emplace_back(v, u, cap);
        edges.emplace_back(u, v, 0);
        adj[v].push_back(m);
        adj[u].push_back(m + 1);
        m += 2;
    }

    bool bfs() {
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (int id : adj[v]) {
                if (edges[id].cap == edges[id].flow)
                    continue;
                if (level[edges[id].u] != -1)
                    continue;
                level[edges[id].u] = level[v] + 1;
                q.push(edges[id].u);
            }
        }
        return level[t] != -1;
    }

    ll dfs(int v, ll pushed) {
        if (pushed == 0)
            return 0;
        if (v == t)
            return pushed;
        for (int& cid = ptr[v]; cid < (int)adj[v].size(); cid++) {
            int id = adj[v][cid];
            int u = edges[id].u;
            if (level[v] + 1 != level[u])
                continue;
            ll tr = dfs(u, min(pushed, edges[id].cap - edges[id].flow));
            if (tr == 0)
                continue;
            edges[id].flow += tr;
            edges[id ^ 1].flow -= tr;
            return tr;
        }
        return 0;
    }

    ll flow() {
        ll f = 0;
        while (true) {
            fill(level.begin(), level.end(), -1);
            level[s] = 0;
            q.push(s);
            if (!bfs())
                break;
            fill(ptr.begin(), ptr.end(), 0);
            while (ll pushed = dfs(s, flow_inf)) {
                f += pushed;
            }
        }
        return f;
    }
};

pair<vector<int>, ll> find_minCut(int s, int t) {
    int n = graph.size();
    Dinic dinic(n, s, t);

    for (int v = 0; v < n; ++v) {
        for (auto [u, cap] : graph[v]) {
            dinic.add_edge(v, u, cap);
        }
    }

    ll cut_value = dinic.flow();

    vector<int> visited(n, 0);
    queue<int> q;
    q.push(s);
    visited[s] = 1;
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (int id : dinic.adj[v]) {
            auto &e = dinic.edges[id];
            if (e.cap > e.flow && !visited[e.u]) {
                visited[e.u] = 1;
                q.push(e.u);
            }
        }
    }

    vector<int> S;
    for (int i = 0; i < n; ++i)
        if (visited[i]) S.push_back(i);

    return {S, cut_value};
}

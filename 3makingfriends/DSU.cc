#include <iostream>
#include <vector>
#include <algorithm>

struct Edge {
    int u, v, w;
};

struct DSU {
    std::vector<int> parent, size;

    DSU(int n){
        parent.resize(n + 1);
        size.assign(n + 1, 1);

        for(int i = 1; i <= n; i++){
            parent[i] = i;
        }
    }

    int find(int x){
        if (parent[x] == x) return x;
            return parent[x] = find(parent[x]);
    }

    bool unite(int a, int b){
        a = find(a);
        b = find(b);

        if (a == b) return false;

        if (size[a] < size[b]){
            std::swap(a, b);
        }

        parent[b] = a;
        size[a] += size[b];
        return true;
    }
};

int main(){
    int N, M;
    std::cin >> N >> M;

    std::vector<Edge> edges;
    edges.reserve(M);

    for(int i = 0; i < M; i++){
        int u, v, w;
        std::cin >> u >> v >> w;
        edges.push_back({u, v, w});
    }

    std::sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b){
        return a.w < b.w;
    });

    DSU dsu(N);

    long long total = 0;
    int edges_used = 0;

    for(const Edge& e : edges){
        if(dsu.unite(e.u, e.v)){
            total += e.w;
            edges_used++;
            if(edges_used == N - 1){
                break;
            }
        }   
    }
    std::cout << total << '\n';
    return 0;
}
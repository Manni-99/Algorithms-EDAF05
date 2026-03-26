import collections

def has_edge(u, v):
    last4 = collections.Counter(u[1:])  # last 4 letters of u
    v_count = collections.Counter(v)
    for char, count in last4.items():
        if v_count[char] < count:
            return False
    return True

def build_graph(words):
    graph = {word: [] for word in words}
    for u in words:
        for v in words:
            if u != v and has_edge(u, v):
                graph[u].append(v)
    return graph

def bfs(graph, root, target):
    if root not in graph or target not in graph:
        return None
    if root == target:
        return 0
    
    visited = {root}
    queue = collections.deque([(root, 0)])
    
    while queue:
        vertex, depth = queue.popleft()
        for neighbour in graph[vertex]:
            if neighbour == target:
                return depth + 1
            if neighbour not in visited:
                visited.add(neighbour)
                queue.append((neighbour, depth + 1))
    
    return None

def main():
    import sys
    data = sys.stdin.read().split()
    idx = 0

    N, Q = int(data[idx]), int(data[idx+1])
    idx += 2

    words = []
    for _ in range(N):
        words.append(data[idx]); idx += 1

    graph = build_graph(words)

    for _ in range(Q):
        src, dst = data[idx], data[idx+1]
        idx += 2
        result = bfs(graph, src, dst)
        print(result if result is not None else "Impossible")

main()
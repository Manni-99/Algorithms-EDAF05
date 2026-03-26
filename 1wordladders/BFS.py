import collections

def has_edge(u, v):
    last4 = collections.Counter(u[1:])  # last 4 letters of u
    v_count = collections.Counter(v)
    for char, count in last4.items(): # O(4) Always 4 iterations
        if v_count[char] < count:
            return False
    return True

def build_graph(words):
    graph = {word: [] for word in words} # Smaller O(N) 
    for u in words: # O(N) iterations
        for v in words: # O(N) iteration for each u
            if u != v and has_edge(u, v): # We compare a single node to everyother node and check whether they are neighbours
                graph[u].append(v)
    return graph # Time complexity O(N² * 4) = O(N²)

def bfs(graph, root, target):
    if root not in graph or target not in graph:
        return None
    if root == target:
        return 0
    
    visited = {root} # First visted node => root
    queue = collections.deque([(root, 0)]) # Initilized queue
    
    while queue: # O(N)
        vertex, depth = queue.popleft() # Iterate the next neighbor
        for neighbour in graph[vertex]: # loop differently for each iteration => O(M) 
            if neighbour == target:
                return depth + 1
            if neighbour not in visited:
                visited.add(neighbour)
                queue.append((neighbour, depth + 1))
    
    return None # Time complexity O(N + M)

def main():
    import sys
    data = sys.stdin.read().split()
    idx = 0 # Cursor of where we are in the list

    N, Q = int(data[idx]), int(data[idx+1]) # Set our number of words and number of queries
    idx += 2    # Move the pointer by 2 -> First word in the dataset

    words = []
    for _ in range(N): # Fill our words array
        words.append(data[idx]); idx += 1

    graph = build_graph(words)

    for _ in range(Q): # Iterate number of queries to check whether BFS is possible
        src, dst = data[idx], data[idx+1] # Set our source word, and destination word
        idx += 2 # move to the next pair
        result = bfs(graph, src, dst) # Check shortest path between the pair
        print(result if result is not None else "Impossible")

main()
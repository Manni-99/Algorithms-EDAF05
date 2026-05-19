/* Railway planning - Lab 6
 * Preflow-push implementation with binary search over route removals.
 */

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRINT 0
#define MIN(a,b) (((a) <= (b)) ? (a) : (b))

#if PRINT
#define pr(...) do { fprintf(stderr, __VA_ARGS__); } while (0)
#else
#define pr(...)
#endif

typedef struct graph_t graph_t;
typedef struct node_t node_t;
typedef struct edge_t edge_t;
typedef struct list_t list_t;
typedef struct input_edge_t input_edge_t;

struct list_t {
    edge_t* edge;
    list_t* next;
};

struct node_t {
    int h;              /* height */
    int e;              /* excess flow */
    int in_excess;      /* true if currently in excess list */
    list_t* edge;       /* adjacency list */
    node_t* next;       /* next node with excess */
};

struct edge_t {
    node_t* u;
    node_t* v;
    int f;              /* positive flow means u -> v, negative means v -> u */
    int c;              /* capacity */
};

struct graph_t {
    int n;
    int m;
    node_t* v;
    edge_t* e;
    node_t* s;
    node_t* t;
    node_t* excess;
};

struct input_edge_t {
    int u;
    int v;
    int c;
};

static char* progname;

#if PRINT
static int id(graph_t* g, node_t* v)
{
    return v - g->v;
}
#endif

static void error(const char* fmt, ...)
{
    va_list ap;
    char buf[BUFSIZ];

    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    if (progname != NULL) {
        fprintf(stderr, "%s: ", progname);
    }

    fprintf(stderr, "error: %s\n", buf);
    exit(1);
}

static int next_int()
{
    int x = 0;
    int c;

    while ((c = getchar()) != EOF && !isdigit(c)) {
        /* skip whitespace */
    }

    if (c == EOF) {
        return 0;
    }

    do {
        x = 10 * x + c - '0';
        c = getchar();
    } while (c != EOF && isdigit(c));

    return x;
}

static void* xmalloc(size_t s)
{
    void* p = malloc(s);

    if (p == NULL) {
        error("out of memory: malloc(%zu) failed", s);
    }

    return p;
}

static void* xcalloc(size_t n, size_t s)
{
    void* p = xmalloc(n * s);
    memset(p, 0, n * s);
    return p;
}

static void add_edge(node_t* u, edge_t* e)
{
    list_t* p = xmalloc(sizeof(list_t));

    p->edge = e;
    p->next = u->edge;
    u->edge = p;
}

/* One undirected railway route with shared capacity.
 * e->f > 0 means flow from u to v.
 * e->f < 0 means flow from v to u.
 */
static void connect(node_t* u, node_t* v, int c, edge_t* e)
{
    e->u = u;
    e->v = v;
    e->c = c;
    e->f = 0;

    add_edge(u, e);
    add_edge(v, e);
}

/* Time complexity: O(n + m)
 * First scans all m input edges to count active edges.
 * Then initializes n nodes.
 * Then scans all m input edges again and connects every non-removed edge.
 */
static graph_t* build_graph(int n, int m, input_edge_t* edges, int* removed)
{
    graph_t* g;
    int active_edges = 0;

    for (int i = 0; i < m; i++) {
        if (!removed[i]) {
            active_edges++;
        }
    }

    g = xmalloc(sizeof(graph_t));

    g->n = n;
    g->m = active_edges;
    g->v = xcalloc(n, sizeof(node_t));
    g->e = xcalloc(active_edges, sizeof(edge_t));

    g->s = &g->v[0];
    g->t = &g->v[n - 1];
    g->excess = NULL;

    int j = 0;

    for (int i = 0; i < m; i++) {
        if (!removed[i]) {
            node_t* u = &g->v[edges[i].u];
            node_t* v = &g->v[edges[i].v];

            connect(u, v, edges[i].c, &g->e[j]);
            j++;
        }
    }

    return g;
}

static void enter_excess(graph_t* g, node_t* v)
{
    if (v != g->t && v != g->s && v->e > 0 && !v->in_excess) {
        v->next = g->excess;
        g->excess = v;
        v->in_excess = 1;
    }
}

static node_t* leave_excess(graph_t* g)
{
    node_t* v = g->excess;

    if (v != NULL) {
        g->excess = v->next;
        v->next = NULL;
        v->in_excess = 0;
    }

    return v;
}

static node_t* other(node_t* u, edge_t* e)
{
    if (u == e->u) {
        return e->v;
    } else {
        return e->u;
    }
}

static void push(graph_t* g, node_t* u, node_t* v, edge_t* e)
{
    int d;

    pr("push from %d to %d: ", id(g, u), id(g, v));
    pr("f = %d, c = %d, so ", e->f, e->c);

    if (u == e->u) {
        d = MIN(u->e, e->c - e->f);
        e->f += d;
    } else {
        d = MIN(u->e, e->c + e->f);
        e->f -= d;
    }

    pr("pushing %d\n", d);

    u->e -= d;
    v->e += d;

    assert(d >= 0);
    assert(u->e >= 0);
    assert(abs(e->f) <= e->c);

    if (u->e > 0) {
        enter_excess(g, u);
    }

    if (v->e == d) {
        enter_excess(g, v);
    }
}

static void relabel(graph_t* g, node_t* u)
{
    u->h += 1;

    pr("relabel %d now h = %d\n", id(g, u), u->h);

    enter_excess(g, u);
}

/* Time complexity: O(n^2 E)
 * This is the standard worst-case bound for the generic preflow-push
 * algorithm, where n is the number of nodes and E is the number of active edges.
 *
 * The initialization loop over the source adjacency list is O(deg(source)).
 * The main loop repeatedly performs pushes and relabels.
 * In the generic push-relabel analysis, the total number of relabel operations
 * and pushes gives the O(n^2 E) worst-case bound.
 */
static int preflow(graph_t* g)
{
    node_t* s;
    node_t* u;
    node_t* v;
    edge_t* e;
    list_t* p;
    int b;

    s = g->s;
    s->h = g->n;

    p = s->edge;

    while (p != NULL) {
        e = p->edge;
        p = p->next;

        s->e += e->c;
        push(g, s, other(s, e), e);
    }

    while ((u = leave_excess(g)) != NULL) {
        v = NULL;
        p = u->edge;

        while (p != NULL) {
            e = p->edge;
            p = p->next;

            if (u == e->u) {
                v = e->v;
                b = 1;
            } else {
                v = e->u;
                b = -1;
            }

            if (u->h > v->h && b * e->f < e->c) {
                break;
            } else {
                v = NULL;
            }
        }

        if (v != NULL) {
            push(g, u, v, e);
        } else {
            relabel(g, u);
        }
    }

    return g->t->e;
}

/* Time complexity: O(n + E)
 * Frees all adjacency-list entries for all n nodes.
 * Since each active undirected edge is stored in two adjacency lists,
 * the total number of list entries is 2E, so this is O(n + E).
 */
static void free_graph(graph_t* g)
{
    int i;
    list_t* p;
    list_t* q;

    for (i = 0; i < g->n; i++) {
        p = g->v[i].edge;

        while (p != NULL) {
            q = p->next;
            free(p);
            p = q;
        }
    }

    free(g->v);
    free(g->e);
    free(g);
}

/* Computes max flow after removing the first k routes in the plan.
 *
 * Time complexity: O(m + k + n + m + n^2 E)
 * - O(m) to allocate and zero removed[]
 * - O(k) to mark the first k planned routes as removed
 * - O(n + m + n^2 E) for compute_flow
 *
 * Since E <= m, this is usually simplified to:
 * O(k + n^2 m)
 */
static int compute_flow(int n, int m, input_edge_t* edges, int* removed)
{
    graph_t* g = build_graph(n, m, edges, removed);
    int f = preflow(g);

    free_graph(g);

    return f;
}

/* Computes max flow after removing the first k routes in the plan. */
static int compute_flow_after_k_removals(
    int n,
    int m,
    input_edge_t* edges,
    int* plan,
    int k
)
{
    int* removed = xcalloc(m, sizeof(int));

    for (int i = 0; i < k; i++) {
        removed[plan[i]] = 1;
    }

    int f = compute_flow(n, m, edges, removed);

    free(removed);

    return f;
}

int main(int argc, char* argv[])
{
    int n;
    int m;
    int C;
    int P;

    input_edge_t* edges;
    int* plan;

    progname = argv[0];

    n = next_int();
    m = next_int();
    C = next_int();
    P = next_int();

    edges = xcalloc(m, sizeof(input_edge_t));

    for (int i = 0; i < m; i++) {
        edges[i].u = next_int();
        edges[i].v = next_int();
        edges[i].c = next_int();
    }

    plan = xcalloc(P, sizeof(int));

    for (int i = 0; i < P; i++) {
        plan[i] = next_int();
    }

    /*
     * We need the largest k such that removing the first k planned routes
     * still gives max flow >= C.
     *
     * This property is monotonic:
     * if removing k routes fails, removing more routes will also fail.
     * Therefore we can binary search instead of testing every k.
     */
    int low = 0;
    int high = P;
    int best = 0;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        int flow = compute_flow_after_k_removals(n, m, edges, plan, mid);

        if (flow >= C) {
            best = mid;
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    int final_flow = compute_flow_after_k_removals(n, m, edges, plan, best);

    printf("%d %d\n", best, final_flow);

    free(edges);
    free(plan);

    return 0;
}
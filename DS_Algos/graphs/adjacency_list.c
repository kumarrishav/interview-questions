#include <stdio.h>
#include <assert.h>

/* ~~~~~~~~~~~~~~~~
 * Linked list library
 * ~~~~~~~~~~~~~~~~
 */
#define MAX_LIST_NODES 256

struct graph_vertex;

struct list_node {
	struct graph_vertex *vertex;
	struct list_node *next;
};

static struct list_node nodes_store[MAX_LIST_NODES];
static size_t next_alloc; // Next node to allocate

void list_insert_after(struct list_node *new_node, struct list_node *after_this) {
	new_node->next = after_this->next;
	after_this->next = new_node;
}

void list_init_node(struct graph_vertex *v, struct list_node *n) {
	assert(n != NULL);

	n->vertex = v;
	n->next = n;
}

struct list_node *new_list_node(struct graph_vertex *vertex) {
	assert(next_alloc < MAX_LIST_NODES);

	struct list_node *ret = &nodes_store[next_alloc];
	list_init_node(vertex, ret);
	next_alloc++;

	return ret;
}

#define list_for_each(head, cursor) \
	for ((cursor) = (head)->next; (cursor) != (head); (cursor) = (cursor)->next)

/* ~~~~~
 * Graph
 * ~~~~~
 */
#define MAX_VERTICES 128

struct graph_vertex {
	int value;
	int visited;
	struct list_node neighbors;
};

struct graph {
	struct graph_vertex vertices[MAX_VERTICES];
	size_t vertices_sz;
};

static void prepare_traversal(struct graph *g) {
	size_t i;
	for (i = 0; i < g->vertices_sz; i++) {
		g->vertices[i].visited = 0;
	}
}

void initialize_graph(struct graph *g, size_t vertices) {
	assert(g != NULL);
	assert(vertices > 0 && vertices <= MAX_VERTICES);

	g->vertices_sz = vertices;

	size_t i;
	for (i = 0; i < g->vertices_sz; i++) {
		g->vertices[i].value = i;
		list_init_node(NULL, &g->vertices[i].neighbors);
	}
}

void add_edge(struct graph *g, size_t from, size_t to) {
	assert(from < g->vertices_sz);
	assert(to < g->vertices_sz);

	struct list_node *node = new_list_node(&g->vertices[to]);

	list_insert_after(node, &g->vertices[from].neighbors);
}

void dfs_aux(struct graph_vertex *v) {
	printf("DFS entered vertex: %d\n", v->value);
	v->visited = 1;
	struct list_node *neighbor_node;

	list_for_each(&v->neighbors, neighbor_node) {
		struct graph_vertex *neighbor = neighbor_node->vertex;
		if (!neighbor->visited) {
			dfs_aux(neighbor);
		}
	}
	printf("DFS exited vertex: %d\n", v->value);
}

void dfs(struct graph *g) {
	prepare_traversal(g);
	size_t i;
	for (i = 0; i < g->vertices_sz; i++) {
		if (!g->vertices[i].visited) {
			dfs_aux(&g->vertices[i]);
		}
	}
}

/* ~~~
 * Driver
 * ~~~
 */
int main(void) {
	printf("Enter number V of vertices, followed by number E of edges,\n");
	printf("followed by E lines of the form: source_node target_node to describe the edges.\n");
	printf("Vertices are numbered from 0 to V-1\n");

	size_t vertices, edges;
	while (scanf("%zu %zu", &vertices, &edges) == 2) {

		struct graph g;
		initialize_graph(&g, vertices);

		while (edges--) {
			size_t from, to;
			scanf("%zu %zu", &from, &to);
			add_edge(&g, from, to);
		}

		printf("~~~ DFS ~~~\n");
		dfs(&g);
		printf("~~~ END DFS ~~~\n\n");

		printf("~~~ BFS ~~~\n");
		// bfs(&g);
		printf("~~~ END BFS ~~~\n");

	}

	return 0;
}

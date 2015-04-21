/* Given a directed graph, design an algorithm to find out whether there is a route
 * between two nodes
 *
 * Source: Cracking the Coding Interview, page 86, question 4.2
 */

#include <stdio.h>
#define MAX_CHILDREN 32
#define MAX_NODES 256

struct graph_node {
	struct graph_node *neighbors[MAX_CHILDREN];
	int visited;
};

static struct graph_node nodes[MAX_NODES];
static size_t nodes_in_use;

void prepare_visit(void) {
	size_t i;
	for (i = 0; i < nodes_in_use; i++)
		nodes[i].visited = 0;
}

int dfs(struct graph_node *from, struct graph_node *to) {
	if (from == to)
		return 1;

	from->visited = 1;

	size_t child = 0;
	while (child < MAX_CHILDREN && from->neighbors[child] != NULL) {
		if (!from->neighbors[child]->visited) {
			if (dfs(from->neighbors[child], to))
				return 1;
		}
		child++;
	}

	return 0;
}

int route_exists(struct graph_node *from, struct graph_node *to) {
	prepare_visit();
	return dfs(from, to);
}

void print_graph() {
	size_t i;
	for (i = 0; i < nodes_in_use; i++) {
		printf("%zu -> ", i);
		size_t child = 0;
		while (child < MAX_CHILDREN && nodes[i].neighbors[child] != NULL) {
			printf("%td ", nodes[i].neighbors[child] - nodes);
			child++;
		}
		printf("\n");
	}
}

int main(void) {

	printf("Input format:\n");
	printf("<number of nodes>\n");
	printf("<number of children of node0> <child1> <child2> ...\n");
	printf("<number of children of node1> <child1> <child2> ... \n");
	printf("node IDs go from 0 to <number of nodes>-1\n");

	scanf("%zu", &nodes_in_use);

	size_t i;
	for (i = 0; i < nodes_in_use; i++) {
		size_t children, j;
		scanf("%zu", &children);
		for (j = 0; j < children; j++) {
			size_t child;
			scanf("%zu", &child);
			nodes[i].neighbors[j] = &nodes[child];
		}
	}

	printf("Now, enter the source and target nodes to see if there's a route\n");

	size_t from, to;
	while (scanf("%zu %zu", &from, &to) == 2)
		printf("Route exists: %s\n", route_exists(&nodes[from], &nodes[to]) ? "yes" : "no");

	return 0;
}

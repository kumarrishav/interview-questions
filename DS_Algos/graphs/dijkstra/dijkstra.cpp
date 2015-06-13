/* Dijkstra's algorithm implementation */

#include <algorithm>
#include <vector>
#include <iostream>
#include <memory>
#include <cassert>
#include <map>
#include <set>
#include <limits>

using namespace std;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ~~~~~~~~~~  Graph ~~~~~~~~~~
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * A graph implementation based on adjacency lists
 *
 */

struct graph_edge;

struct graph_node {
	int id;
	vector<graph_edge> neighbors;
	bool visited;
	unsigned distance; // Used by Dijkstra
	graph_node *prev; // Used by Dijkstra

	graph_node(void) { }
	graph_node(int identifier) : id(identifier) { }
};

struct graph_edge {
	unsigned cost;
	graph_node *target;
	graph_edge(unsigned c, graph_node *dst) : cost(c), target(dst) { }
	graph_edge(void) { }
};

class DistComparator {
public:
	/* Sets cannot have more than one element with the same key,
	 * so we need to use the node ID to untie
	 */
	bool operator()(graph_node *a, graph_node *b) {
		return (a->distance < b->distance) || (a->distance == b->distance && a->id < b->id);
	}
};

class Graph {
private:
	map<int, graph_node> nodes;

	void dfs_visit(graph_node *node) {
		node->visited = true;
		cout << node->id << endl;
		for (vector<graph_edge>::iterator child_it = node->neighbors.begin();
		     child_it != node->neighbors.end();
		     child_it++) {
			if (!child_it->target->visited) {
				dfs_visit(child_it->target);
			}
		}
	}

	void print_path(const graph_node *src, const graph_node *curr) {
		if (curr == src) {
			cout << src->id;
			return;
		}
		print_path(src, curr->prev);
		cout << " -> " << curr->id;
	}

	void print_shortest_paths(int source) {
		graph_node *src = &nodes[source];
		for (map<int, graph_node>::const_iterator node_it = nodes.begin();
		     node_it != nodes.end();
		     node_it++) {
			if (node_it->second.prev != NULL) {
				print_path(src, &node_it->second);
				cout << endl;
			}
		}
	}

public:
	typedef typename map<int, graph_node>::size_type size_type;

	Graph(void) { }

	void add_node(int id) {
		nodes[id] = graph_node(id);
	}

	void add_edge(int from, int to, int cost) {
		graph_node *src = &nodes[from];
		graph_node *dst = &nodes[to];
		src->neighbors.push_back(graph_edge(cost, dst));
	}

	void dfs(void) {
		for (map<int, graph_node>::iterator node_it = nodes.begin();
		     node_it != nodes.end();
		     node_it++) {
			node_it->second.visited = false;
		}
		for (map<int, graph_node>::iterator node_it = nodes.begin();
		     node_it != nodes.end();
		     node_it++) {
			if (!node_it->second.visited) {
				dfs_visit(&node_it->second);
			}
		}
	}

	void dijkstra(int source) {
		set<graph_node*, DistComparator> queue;

		for (map<int, graph_node>::iterator node_it = nodes.begin();
		     node_it != nodes.end();
		     node_it++) {
			node_it->second.prev = NULL;
			if (node_it->second.id == source) {
				node_it->second.distance = 0;
			} else {
				node_it->second.distance = numeric_limits<unsigned>::max();
			}
			queue.insert(&node_it->second);
		}

		while (!queue.empty()) {
			graph_node *n = *queue.begin();
			queue.erase(queue.begin());
			for (vector<graph_edge>::iterator edge_it = n->neighbors.begin();
			     edge_it != n->neighbors.end();
			     edge_it++)  {

				set<graph_node*, DistComparator>::iterator target_it;
				target_it = queue.find(edge_it->target);

				/* We test n->distance != UINT_MAX because UINT_MAX
				 * simulates infinity, and x+infinity == infinity
				 * (i.e. we don't want to wrap around on unreachable nodes)
				 */

				if (target_it != queue.end() &&
				    n->distance != numeric_limits<unsigned>::max() &&
				    n->distance+edge_it->cost < edge_it->target->distance) {

					queue.erase(target_it);
					edge_it->target->distance = n->distance+edge_it->cost;
					edge_it->target->prev = n;
					queue.insert(edge_it->target);

				}
			}
		}

		print_shortest_paths(source);
	}

	size_type size(void) {
		return nodes.size();
	}
};

int main(void) {

	cout << "Enter number of nodes, followed by each node id, followed by:" << endl;
	cout << "id number_of_neighbors neighbor1_id cost_to_neighbor1 neighbor2_id cost_to_neighbor2 ... neighborN_id cost_to_neighborN" << endl;
	cout << "Followed by the source" << endl;
	cout << "> ";

	Graph::size_type nodes;
	while (cin >> nodes) {
		Graph graph;

		for (Graph::size_type i = 0; i < nodes; i++) {
			int id;
			cin >> id;
			graph.add_node(id);
		}

		for (Graph::size_type i = 0; i < nodes; i++) {
			int id;
			cin >> id;

			Graph::size_type neighbors;
			cin >> neighbors;
			for (Graph::size_type j = 0; j < neighbors; j++) {
				int neighbor, cost;
				cin >> neighbor >> cost;
				graph.add_edge(id, neighbor, cost);
			}
		}

		int src;
		cin >> src;

		cout << "Performing a DFS visit" << endl;
		graph.dfs();

		cout << "Calculating shortest paths from " << src << endl;
		graph.dijkstra(src);

		cout << "> ";
	}

	return 0;
}

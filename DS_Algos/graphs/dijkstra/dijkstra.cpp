#include <algorithm>
#include <vector>
#include <iostream>
#include <memory>
#include <cassert>
#include <map>

using namespace std;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ~~~~~~~~~~  Priority Queue ~~~~~~~~~~
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * This is a priority queue implementation based on a min-heap
 *
 * Insertion: O(log(n))
 * Deletion: O(log(n))
 * Peek: O(1)
 * IsEmpty: O(1)
 * Size: O(1)
 */

template<class T>
class PriorityQueue {
private:
	vector<T> min_heap;

	void sift_down(void) {
		typename vector<T>::size_type curr = 0;
		typename vector<T>::size_type left = curr*2+1;
		typename vector<T>::size_type right = left+1;
		typename vector<T>::size_type smallest = curr;

		if (left < min_heap.size() && min_heap[left] < min_heap[smallest]) {
			smallest = left;
		}
		if (right < min_heap.size() && min_heap[right] < min_heap[smallest]) {
			smallest = right;
		}

		while (curr != smallest) {
			swap(min_heap[curr], min_heap[smallest]);
			curr = smallest;
			left = curr*2+1;
			right = left+1;

			if (left < min_heap.size() && min_heap[left] < min_heap[smallest]) {
				smallest = left;
			}
			if (right < min_heap.size() && min_heap[right] < min_heap[smallest]) {
				smallest = right;
			}

		}
	}

	void sift_up(void) {
		typename vector<T>::size_type curr = min_heap.size()-1;
		typename vector<T>::size_type parent = (curr+1)/2-1;

		while (curr != 0 && min_heap[curr] < min_heap[parent]) {
			swap(min_heap[curr], min_heap[parent]);
			curr = parent;
			parent = (curr+1)/2-1;
		}

	}

	T extract_min(void) {
		swap(min_heap[0], min_heap[min_heap.size()-1]);
		T ret = min_heap[min_heap.size()-1];
		min_heap.pop_back();
		sift_down();
		return ret;
	}

	void insert(const T &element) {
		min_heap.push_back(element);
		sift_up();
	}

	T peek_min(void) {
		return min_heap[0];
	}

public:

	typedef typename vector<T>::size_type size_type;

	PriorityQueue(void) { }

	T dequeue(void) {
		return extract_min();
	}

	void enqueue(const T &element) {
		insert(element);
	}

	T peek(void) {
		return peek_min();
	}

	bool empty(void) {
		return min_heap.size() == 0;
	}

	size_type size(void) {
		return min_heap.size();
	}
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ~~~~~~~~~~  End of Priority Queue ~~~~~~~~~~
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

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
	graph_node(void) { }
	graph_node(int identifier) : id(identifier) { }
};

struct graph_edge {
	unsigned cost;
	graph_node *target;
	graph_edge(unsigned c, graph_node *dst) : cost(c), target(dst) { }
	graph_edge(void) { }
};

class Graph {
private:
	map<int, graph_node> nodes;

	void dfs_visit(graph_node *node) {
		node->visited = true;
		cout << node->id << endl;
		for (vector<graph_edge>::iterator child_it = node->neighbors.begin(); child_it != node->neighbors.end(); child_it++) {
			if (!child_it->target->visited) {
				dfs_visit(child_it->target);
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
		for (map<int, graph_node>::iterator node_it = nodes.begin(); node_it != nodes.end(); node_it++) {
			node_it->second.visited = false;
		}
		for (map<int, graph_node>::iterator node_it = nodes.begin(); node_it != nodes.end(); node_it++) {
			if (!node_it->second.visited) {
				dfs_visit(&node_it->second);
			}
		}
	}

	size_type size(void) {
		return nodes.size();
	}
};

int main(void) {

	cout << "Enter number of nodes, followed by each node id, followed by:" << endl;
	cout << "id number_of_neighbors neighbor1_id cost_to_neighbor1 neighbor2_id cost_to_neighbor2 ... neighborN_id cost_to_neighborN" << endl;
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

		graph.dfs();

		cout << "> ";
	}

	return 0;
}

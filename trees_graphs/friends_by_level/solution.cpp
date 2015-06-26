/* Given a person and a list of his friends, print all his friends by level of
 * association.
 *
 * EXAMPLE
 *
 * Consider these friendship relations:
 *
 * A: B, C, D
 * D: B, E, F
 * E: C, F, G
 *
 * This means that person A is friends with B, C and D.
 * Person D is friends with B, E and F.
 * Etc...
 *
 * If the source person is A, then the output will be:
 *
 * Level 1: B C D
 * Level 2: E F
 * Level 3: G
 *
 * Source: Careercup (Amazon interview)
 */

#include <iostream>
#include <vector>
#include <queue>
#include <map>

using namespace std;

struct graph_node {
	char id;
	vector<graph_node *> adj;
	bool visited;
};

void friends_by_level(map<char, graph_node*> &graph, graph_node *source) {
	for (map<char, graph_node *>::const_iterator it = graph.begin(); it != graph.end(); it++) {
		it->second->visited = false;
	}

	source->visited = true;

	queue<graph_node*> bfsq;
	unsigned curr_level = 0;
	unsigned level = 1;

	for (vector<graph_node*>::iterator it = source->adj.begin(); it != source->adj.end(); it++) {
		(*it)->visited = true;
		curr_level++;
		bfsq.push(*it);
	}

	while (!bfsq.empty()) {
		unsigned next_level = 0;
		cout << "Level " << level << ": ";
		for (unsigned i = 0; i < curr_level; i++) {
			graph_node *friend_node = bfsq.front();
			bfsq.pop();
			cout << friend_node->id << " ";
			for (vector<graph_node*>::iterator it = friend_node->adj.begin();
			     it != friend_node->adj.end();
			     it++) {
				if (!(*it)->visited) {
					(*it)->visited = true;
					bfsq.push(*it);
					next_level++;
				}
			}
		}
		cout << endl;
		curr_level = next_level;
		level++;
	}
}

static graph_node *get_node(map<char, graph_node *> &graph, char node) {
	if (graph.find(node) == graph.end()) {
		graph[node] = new graph_node;
		graph[node]->id = node;
	}
	return graph[node];
}

int main(void) {
	cout << "Enter: size N of graph, followed by N lines, where each line is of the form:" << endl;
	cout << "I C c1 c2 c3 ... cN" << endl;
	cout << "Where I is a node's ID, C is the number of friends, and c1, c2, ..., cN are the IDs of I's friends" << endl;
	cout << "Followed by the source person" << endl;
	cout << "> ";

	map<char, graph_node*>::size_type graph_sz;
	while (cin >> graph_sz) {
		map<char, graph_node *> graph;
		for (map<char, graph_node*>::size_type i = 0; i < graph_sz; i++) {
			char nodei;
			cin >> nodei;

			graph_node *node = get_node(graph, nodei);
			map<char, graph_node*>::size_type friends_count;
			cin >> friends_count;

			for (map<char, graph_node*>::size_type j = 0; j < friends_count; j++) {
				char fid;
				cin >> fid;
				graph_node *f = get_node(graph, fid);
				node->adj.push_back(f);
			}
		}

		char source;
		cin >> source;
		friends_by_level(graph, get_node(graph, source));

		for (map<char, graph_node*>::iterator it = graph.begin(); it != graph.end(); it++) {
			delete it->second;
		}

		cout << "> ";
	}

	return 0;
}

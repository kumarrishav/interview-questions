/* You are given a set of tasks and their dependencies.
 * A task can only be performed once all of its dependencies have been executed
 * Design and implement an algorithm that returns a possible ordering of the tasks
 * that doesn't break dependencies.
 *
 * EXAMPLE
 * If task 1 depends on task 4 and 5, and task 4 depends on task 3, a possible ordering
 * is 3, 4, 5, 1
 *
 * Source: past interview experience
 */
#include <iostream>
#include <vector>
#include <map>

using namespace std;

struct graph_node {
	int task_id;
	vector<graph_node*> neighbors;
	bool visited;
};

static void dfs_visit(graph_node *node, vector<int> &result) {
	node->visited = true;
	for (vector<graph_node*>::const_iterator child_it = node->neighbors.begin();
	     child_it != node->neighbors.end();
	     child_it++) {
		if (!(*child_it)->visited) {
			dfs_visit(*child_it, result);
		}
	}
	result.push_back(node->task_id);
}

vector<int> make_schedule(map<int, graph_node*> &nodes) {
	for (map<int, graph_node*>::const_iterator node_it = nodes.begin();
	     node_it != nodes.end();
	     node_it++) {
		node_it->second->visited = false;
	}

	vector<int> result;

	for (map<int, graph_node*>::const_iterator node_it = nodes.begin();
	     node_it != nodes.end();
	     node_it++) {
		if (!node_it->second->visited) {
			dfs_visit(node_it->second, result);
		}
	}

	return result;
}

static map<int, graph_node*> graph_nodes;

static void clear_graph_nodes(void) {
	for (map<int, graph_node*>::const_iterator it = graph_nodes.begin();
	     it != graph_nodes.end();
	     it++) {
		delete it->second;
	}
	graph_nodes.clear();
}

graph_node *get_node(int nid) {
	if (graph_nodes.find(nid) == graph_nodes.end()) {
		graph_nodes[nid] = new graph_node;
		graph_nodes[nid]->task_id = nid;
	}
	return graph_nodes[nid];
}

int main(void) {
	cout << "Enter the number of tasks, and for each task, enter a line of the form:" << endl;
	cout << "<task_id> <number of dependencies> <dependency1 task id> <...> <dependencyN task id>" << endl;
	cout << "> ";

	vector<int>::size_type tasks_count;
	while (cin >> tasks_count) {

		for (vector<int>::size_type i = 0; i < tasks_count; i++) {
			int tid;
			cin >> tid;
			graph_node *gnode = get_node(tid);

			int deps;
			cin >> deps;
			for (int i = 0; i < deps; i++) {
				int dep_id;
				cin >> dep_id;
				gnode->neighbors.push_back(get_node(dep_id));
			}
		}

		vector<int> schedule = make_schedule(graph_nodes);
		cout << "Order: " << schedule[0];
		for (vector<int>::size_type i = 1; i < schedule.size(); i++) {
			cout << ", " << schedule[i];
		}
		cout << endl;

		clear_graph_nodes();

		cout << "> ";
	}

	return 0;
}

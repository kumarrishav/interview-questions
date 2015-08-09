/* Write a function that analyzes a list of package dependencies and prints a valid package
 * installation order according to the dependencies.
 *
 * The function receives as input an array of strings with the following format:
 *
 * package_name: dependency1, dependency2, dependency3, ..., dependencyN
 *
 * If a package has no dependencies, the entry will be:
 *
 * package_name:
 *
 * If there are cyclic dependencies, print "Dependencies contain cycles"
 *
 * EXAMPLE
 *
 * Input:
 *
 * emacs: gcc, build-essential, g++
 * g++: build-essential, vi
 *
 * Output:
 *
 * build-essential, vi, g++, gcc, emacs
 *
 * (Or any other valid order)
 *
 * Explanation: to install emacs, we must have gcc, build-essential and g++. To install g++, we must
 *              have build-essential and vi. Thus, one possibility is to install build-essential,
 *              then install vi, then g++, followed by gcc and finally emacs.
 *
 * Source: Careercup
 */

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <cassert>
#include <cctype>

using namespace std;

struct graph_node {
	string package;
	vector<graph_node*> neighbors;
	bool visited;
	bool in_stack;
	graph_node(const string &pkg_name) : package(pkg_name) { }
};

static graph_node *get_graph_node(map<string, graph_node *> &graph, const string &str) {
	if (graph.find(str) == graph.end())
		graph[str] = new graph_node(str);
	return graph[str];
}

static bool dfs_visit(graph_node *node, vector<graph_node *> &top_sort_out) {
	if (node->in_stack)
		return false;
	if (node->visited)
		return true;

	node->visited = true;
	node->in_stack = true;

	for (vector<graph_node *>::iterator neighbor_it = node->neighbors.begin();
	     neighbor_it != node->neighbors.end();
	     neighbor_it++)
		if (!dfs_visit(*neighbor_it, top_sort_out))
			return false;

	top_sort_out.push_back(node);
	node->in_stack = false;

	return true;
}

static string topological_sort(map<string, graph_node *> &graph) {
	for (map<string, graph_node*>::iterator node_it = graph.begin();
	     node_it != graph.end();
	     node_it++) {
		node_it->second->visited = false;
		node_it->second->in_stack = false;
	}

	vector<graph_node *> deps_order;

	for (map<string, graph_node*>::iterator node_it = graph.begin();
	     node_it != graph.end();
	     node_it++) {
		if (!node_it->second->visited && !dfs_visit(node_it->second, deps_order))
			return "Dependencies contain cycles";
	}

	ostringstream res;

	if (deps_order.size() > 0)
		res << deps_order[0]->package;

	for (vector<graph_node *>::size_type i = 1; i < deps_order.size(); i++)
		res << ", " << deps_order[i]->package;

	return res.str();
}

void parse_dependency(const string &entry, string &pkg_name, vector<string> &deps) {
	string::size_type pkg_name_end = entry.find(':');
	assert(pkg_name_end < entry.size());
	pkg_name = entry.substr(0, pkg_name_end);

	string::size_type begin = pkg_name_end+1;
	while (begin < entry.size()) {
		while (begin < entry.size() && isspace(entry[begin]))
			begin++;

		string::size_type end = begin;

		while (end < entry.size() && entry[end] != ',' && !isspace(entry[end]))
			end++;

		if (begin < entry.size())
			deps.push_back(entry.substr(begin, end-begin));

		begin = end+1;
	}
}

void print_dependencies(const vector<string> &packages) {
	map<string, graph_node *> graph;

	/* Build the graph */
	for (vector<string>::const_iterator pkg_it = packages.begin();
	     pkg_it != packages.end();
	     pkg_it++) {

		string pkg_name;
		vector<string> deps;

		parse_dependency(*pkg_it, pkg_name, deps);
		graph_node *node = get_graph_node(graph, pkg_name);

		for (vector<string>::const_iterator dep_it = deps.begin();
		     dep_it != deps.end();
		     dep_it++)
			node->neighbors.push_back(get_graph_node(graph, *dep_it));
	}

	/* Perform topological sort, or stop if there is a cycle */
	string res = topological_sort(graph);
	cout << res << endl;
}

int main(void) {
	cout << "Enter the number of entries, followed by each entry." << endl;
	cout << "An entry is of the form: package_name: dependency1, dependency2, ..." << endl;
	cout << "Example: emacs: gcc, build-essential, g++" << endl;
	cout << "> ";

	unsigned entries_count;
	while (cin >> entries_count) {

		vector<string> entries(entries_count);
		getline(cin, entries[0]); // Ignore last newline

		for (unsigned i = 0; i < entries_count; i++)
			getline(cin, entries[i]);

		print_dependencies(entries);
		cout << "> ";
	}

	return 0;
}
